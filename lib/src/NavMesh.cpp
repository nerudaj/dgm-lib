#include "DGM/classes/NavMesh.hpp"
#include "DGM/classes/Error.hpp"
#include <cmath>
#include <map>

namespace custom {
    constexpr [[nodiscard]] unsigned max(unsigned a, unsigned b) noexcept {
        return a < b ? b : a;
    }

    constexpr [[nodiscard]] unsigned min(unsigned a, unsigned b) noexcept {
        return a < b ? a : b;
    }

    constexpr [[nodiscard]] unsigned abs(unsigned a, unsigned b) noexcept {
        return max(a, b) - min(a, b);
    }
}

namespace std {
    template<>
    struct less<sf::Vector2u> {
        bool operator()(const sf::Vector2u& a, const sf::Vector2u& b) const {
            return a.x <= b.x && a.y <= b.y;
        }
    };
}

enum class Dir {
    Up, Left, Down, Right
};

struct Node {
protected:
    static constexpr [[nodiscard]] unsigned getDistance(const sf::Vector2u& a, const sf::Vector2u& b) noexcept {
        return custom::abs(a.x, b.x) + custom::abs(a.y, b.y);
    }

public:
    sf::Vector2u point = {};
    unsigned gcost = 0;
    unsigned hcost = 0;
    unsigned fcost = 0;
    Dir backdir = Dir::Up;

    Node() {}
    Node(const sf::Vector2u &point, const sf::Vector2u &end, unsigned gcost, Dir backdir) {
        this->point = point;
        this->backdir = backdir;
        this->gcost = gcost;
        hcost = getDistance(point, end);
        fcost = gcost + hcost;
    }
    Node(const Node& other) = default;

    [[nodiscard]] bool isCloserThan(const Node& other) const noexcept {
        return fcost < other.fcost || (fcost == other.fcost && hcost < other.hcost);
    }
};

class NodeSet {
protected:
    std::map<sf::Vector2u, Node> nodes;

public:
    void insertNode(const Node& node) {
        nodes[node.point] = node;
    }

    [[nodiscard]] bool isEmpty() const noexcept {
        return nodes.empty();
    }

    [[nodiscard]] bool contains(const sf::Vector2u& p) const noexcept {
        return nodes.find(p) != nodes.end();
    }

    [[nodiscard]] Node popBestNode() {
        auto minElem = nodes.begin();
        for (auto itr = (++nodes.begin()); itr != nodes.end(); itr++) {
            if (itr->second.isCloserThan(minElem->second)) {
                minElem = itr;
            }
        }

        Node copy = minElem->second;
        nodes.erase(minElem);
        return copy;
    }

    [[nodiscard]] Node getNode(const sf::Vector2u& point) const {
        return nodes.at(point);
    }
};

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

dgm::Path<dgm::TileNavpoint> dgm::NavMesh::getPath(const sf::Vector2u& from, const sf::Vector2u& to) {
    if (from == to)
        dgm::Path<TileNavpoint>({}, false);

    NodeSet openSet, closedSet;

    closedSet.insertNode(Node(from, to, 0, Dir::Down));
    openSet.insertNode(Node(sf::Vector2u(from.x, from.y - 1), to, 1, Dir::Down));
    openSet.insertNode(Node(sf::Vector2u(from.x, from.y + 1), to, 1, Dir::Up));
    openSet.insertNode(Node(sf::Vector2u(from.x - 1, from.y), to, 1, Dir::Right));
    openSet.insertNode(Node(sf::Vector2u(from.x + 1, from.y), to, 1, Dir::Left));

    auto canVisit = [&] (const sf::Vector2u& p) {
        return !closedSet.contains(p) && mesh.at(p.x, p.y) <= 0;
    };

    bool success = false;
    while (!openSet.isEmpty()) {
        Node node = openSet.popBestNode();
        closedSet.insertNode(node);
        if (node.point == to) {
            success = true;
            break;
        }

        sf::Vector2u p1(node.point.x, node.point.y - 1);
        sf::Vector2u p2(node.point.x, node.point.y + 1);
        sf::Vector2u p3(node.point.x - 1, node.point.y);
        sf::Vector2u p4(node.point.x + 1, node.point.y);

        if (canVisit(p1)) openSet.insertNode(Node(p1, to, node.gcost + 1, Dir::Down));
        if (canVisit(p2)) openSet.insertNode(Node(p2, to, node.gcost + 1, Dir::Up));
        if (canVisit(p3)) openSet.insertNode(Node(p3, to, node.gcost + 1, Dir::Right));
        if (canVisit(p4)) openSet.insertNode(Node(p4, to, node.gcost + 1, Dir::Left));
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
        points.push_back(TileNavpoint(to, 0u));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path(points, false);
}

dgm::NavMesh::NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {
    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point);
}
