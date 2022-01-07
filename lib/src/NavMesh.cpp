#include "DGM/classes/NavMesh.hpp"
#include "DGM/classes/Error.hpp"

#include <queue>

void dgm::NavMesh::updateOpenSetWithCoord(NodeSet& openSet, const sf::Vector2u& coord, const NodeSet& closedSet, const sf::Vector2u& destinationCoord) {
    const Node node = closedSet.getNode(coord);

    auto canVisit = [&] (const sf::Vector2u& p) {
        const bool notInClosedSet = !closedSet.contains(p);
        const bool emptyInMesh = mesh.at(p.x, p.y) <= 0;
        return notInClosedSet && emptyInMesh;
    };

    const sf::Vector2u p1(node.point.x, node.point.y - 1);
    const sf::Vector2u p2(node.point.x, node.point.y + 1);
    const sf::Vector2u p3(node.point.x - 1, node.point.y);
    const sf::Vector2u p4(node.point.x + 1, node.point.y);

    if (canVisit(p1))
        openSet.insertNode(Node(p1, destinationCoord, node.gcost + 1, Dir::Down));
    if (canVisit(p2))
        openSet.insertNode(Node(p2, destinationCoord, node.gcost + 1, Dir::Up));
    if (canVisit(p3))
        openSet.insertNode(Node(p3, destinationCoord, node.gcost + 1, Dir::Right));
    if (canVisit(p4))
        openSet.insertNode(Node(p4, destinationCoord, node.gcost + 1, Dir::Left));
}

dgm::Path<dgm::TileNavpoint> dgm::NavMesh::getPath(const sf::Vector2u& from, const sf::Vector2u& to) {
    if (from == to)
        return dgm::Path<TileNavpoint>({}, false);

    NodeSet openSet, closedSet;

    closedSet.insertNode(Node(from, to, 0, Dir::Down));
    updateOpenSetWithCoord(openSet, from, closedSet, to);

    bool success = false;
    while (!openSet.isEmpty()) {
        Node node = openSet.popBestNode();
        closedSet.insertNode(node);

        if (node.point == to) {
            success = true;
            break;
        }

        updateOpenSetWithCoord(openSet, node.point, closedSet, to);
    }

    if (!success)
        throw dgm::GeneralException("No path was found");
    
    std::vector<TileNavpoint> points;
    points.push_back(TileNavpoint(to, 0u));

    sf::Vector2u point = to;
    do {
        Node node = closedSet.getNode(point);
        switch (node.backdir) {
        case Dir::Up:
            --point.y; break;
        case Dir::Left:
            --point.x; break;
        case Dir::Down:
            ++point.y; break;
        case Dir::Right:
            ++point.x; break;
        }

        if (point == from) break;
        points.push_back(TileNavpoint(point, 0u));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path(points, false);
}

dgm::NavMesh::NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {}

// ====================

enum class SeekDir : std::uint8_t {
    // Upper two bits are used to denote vertical/horizontal direction
    // If both bits are toggled, then its diagonal. It will be useful
    // to reduce code for testing neighbours
    // Lower bits are just iterating unique values for each code
    Up = 0b10000001,
    UpRight = 0b11000010,
    Right = 0b01000011,
    DownRight = 0b11000100,
    Down = 0b10000101,
    DownLeft = 0b11000110,
    Left = 0b01000111,
    UpLeft = 0b11001000
};

void dgm::WorldNavMesh::discoverConnectionsForJumpPoint(const sf::Vector2u& point, bool fullSearch) {
    using Seeker = std::pair<sf::Vector2u, SeekDir>;

    auto isJumpPoint = [&] (const sf::Vector2u& p) {
        return jumpPointConnections.find(p) != jumpPointConnections.end();
    };

    // point is current seeker point, origin is jump point from which connections are being
    // discovered
    auto isHorizontalNeighborOk = [&] (const sf::Vector2u &point, const sf::Vector2u& origin) {
        // If point is not on the same axis as origin, then we need to check horizontal neighbor
        // that is closer to origin. If it is solid wall then direct visibility to point is compromised
        // and function returns false
        if (point.x < origin.x && mesh.at(point.x + 1, point.y) > 1) return false;
        else if (point.x > origin.x && mesh.at(point.x - 1, point.y) > 1) return false;
        return true;
    };

    auto isVerticalNeighborOk = [&] (const sf::Vector2u& point, const sf::Vector2u& origin) {
        // Same as test for horizontal neighbour, just for Y axis
        if (point.y < origin.y && mesh.at(point.x, point.y + 1) > 1) return false;
        else if (point.y > origin.y && mesh.at(point.x, point.y - 1) > 1) return false;
        return true;
    };

    auto isVerticalSeekDir = [&] (SeekDir dir) {
        return static_cast<uint8_t>(dir) & 0b10000000;
    };

    auto isHorizontalSeekDir = [&] (SeekDir dir) {
        return static_cast<uint8_t>(dir) & 0b01000000;
    };

    auto connectTwoJumpPoints = [&] (const sf::Vector2u& a, const sf::Vector2u& b) {
        const float dx = (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
        const float dy = (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        jumpPointConnections[a].push_back(Connection(b, distance));
        jumpPointConnections[b].push_back(Connection(a, distance));
    };

    std::queue<Seeker> seekers;

    // For discovery of basic jump points, we only need a partial search since
    // everything is symmetrical
    seekers.push({ sf::Vector2u(point.x + 1, point.y - 1), SeekDir::UpRight });
    seekers.push({ sf::Vector2u(point.x + 1, point.y), SeekDir::Right });
    seekers.push({ sf::Vector2u(point.x + 1, point.y + 1), SeekDir::DownRight });
    seekers.push({ sf::Vector2u(point.x, point.y + 1), SeekDir::Down });

    // For source/end points of actual pathfinding, we need full search
    if (fullSearch) {
        seekers.push({ sf::Vector2u(point.x, point.y - 1), SeekDir::Up });
        seekers.push({ sf::Vector2u(point.x + 1, point.y - 1), SeekDir::UpLeft });
        seekers.push({ sf::Vector2u(point.x + 1, point.y), SeekDir::Left });
        seekers.push({ sf::Vector2u(point.x + 1, point.y + 1), SeekDir::DownLeft });
        seekers.push({ sf::Vector2u(point.x, point.y + 1), SeekDir::Down });
    }

    while (!seekers.empty()) {
        const auto [coord, dir] = seekers.front(); seekers.pop();

        // Skip if seeker hit a wall or neighbor compromises visibility
        if (mesh.at(coord) > 0) continue;
        if (isVerticalSeekDir(dir) && !isVerticalNeighborOk(coord, point)) continue;
        if (isHorizontalSeekDir(dir) && !isHorizontalNeighborOk(coord, point)) continue;

        // If coord is valid jump point, do not continue search from there
        if (isJumpPoint(coord)) {
            connectTwoJumpPoints(point, coord);
            continue;
        }

        // Seeker didn't find anything and visibility not compromised, spawn new seekers
        switch (dir) {
        case SeekDir::Up:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), dir }); break;
        case SeekDir::Left:
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), dir }); break;
        case SeekDir::Down:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), dir }); break;
        case SeekDir::Right:
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y), dir }); break;
        case SeekDir::UpLeft:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), SeekDir::Up });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), SeekDir::Left });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y - 1), dir });
            break;
        case SeekDir::UpRight:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), SeekDir::Up });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y), SeekDir::Right });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y - 1), dir });
            break;
        case SeekDir::DownLeft:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), SeekDir::Down });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), SeekDir::Left });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y + 1), dir });
            break;
        case SeekDir::DownRight:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), SeekDir::Down });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y), SeekDir::Right });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y + 1), dir });
            break;
        }
    }
}

dgm::WorldNavMesh::WorldNavMesh(const dgm::Mesh& mesh) : mesh(mesh) {
    auto isJumpPoint = [&] (const sf::Vector2u& point) {
       /**
        *  Test if this point is at the tip of some impassable tile, eg:
        *  #   #
        *  # p     <-- there the p is corner point
        *  # # #
        *
        *
        *  #   #
        *  # p #   <-- there p is not a corner point
        *  #   #
        *
        *  #   #
        *    p     <-- multiple corners
        *  # # #
        */

        const bool northOpened = mesh.at(point.x, point.y - 1) <= 0;
        const bool westOpened = mesh.at(point.x - 1, point.y) <= 0;
        const bool southOpened = mesh.at(point.x, point.y + 1) <= 0;
        const bool eastOpened = mesh.at(point.x + 1, point.y) <= 0;
        const bool northWestCorner = mesh.at(point.x - 1, point.y - 1) > 0 && westOpened && northOpened;
        const bool northEastCorner = mesh.at(point.x + 1, point.y - 1) > 0 && eastOpened && northOpened;
        const bool southWestCorner = mesh.at(point.x - 1, point.y + 1) > 0 && westOpened && southOpened;
        const bool southEastCorner = mesh.at(point.x + 1, point.y + 1) > 0 && eastOpened && southOpened;

        return northWestCorner || northEastCorner || southWestCorner || southEastCorner;
    };

    auto discoverJumpPoints = [&] () {
        for (unsigned y = 1; y < mesh.getDataSize().y - 1; y++) {
            for (unsigned x = 1; x < mesh.getDataSize().x - 1; x++) {
                // Skip impassable blocks
                if (mesh.at(x, y) > 0) continue;

                sf::Vector2u point(x, y);
                if (isJumpPoint(point)) {
                    jumpPointConnections[point] = {};
                }
            }
        }
    };

    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point, false);
}
