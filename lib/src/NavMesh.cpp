#include "DGM/classes/NavMesh.hpp"
#include "DGM/classes/Error.hpp"

bool dgm::NavMesh::isJumpPoint(const sf::Vector2u& point) noexcept {
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
}

void dgm::NavMesh::connectTwoJumpPoints(const sf::Vector2u& a, const sf::Vector2u& b) {
    const float dx = (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
    const float dy = (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    jumpPointConnections[a].push_back(Connection(a, distance));
    jumpPointConnections[b].push_back(Connection(b, distance));
}

void dgm::NavMesh::discoverJumpPoints() {
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
}

void dgm::NavMesh::discoverConnectionsForJumpPoint(const sf::Vector2u& point) {
    // TODO: this
}

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

dgm::NavMesh::NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {
    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point);
}
