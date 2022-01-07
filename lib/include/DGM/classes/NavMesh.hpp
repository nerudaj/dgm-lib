#include <SFML/System/Vector2.hpp>
#include <DGM/classes/Path.hpp>
#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Utility.hpp>

#include <unordered_map>
#include <cmath>
#include <map>

namespace std {
    template<>
    struct hash<sf::Vector2u> {
        std::size_t operator()(const sf::Vector2u &vec) const {
            using Unsigned = decltype(vec.x);
            if constexpr (sizeof(Unsigned) * 2 <= sizeof(std::size_t)) {
                return (static_cast<std::size_t>(vec.x) << (sizeof(Unsigned) * 8)) + vec.y;
            }
            else {
                return vec.x ^ vec.y;
            }
        }
    };
};

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

namespace dgm {

/**
 *  \brief Class for computing path from collision mesh data
 */
class NavMesh {
public:
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
        Node(const sf::Vector2u& point, const sf::Vector2u& end, unsigned gcost, Dir backdir) {
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
        std::map<sf::Vector2u, Node, dgm::Utility::less<sf::Vector2u>> nodes;

    public:
        void insertNode(const Node& node) {
            nodes[node.point] = node;
        }

        [[nodiscard]] bool isEmpty() const noexcept {
            return nodes.empty();
        }

        [[nodiscard]] bool contains(const sf::Vector2u& p) const noexcept {
            const auto itr = nodes.find(p);
            return itr != nodes.end();
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

protected:
    dgm::Mesh mesh;

protected: // WorldNavpoint helpers
    [[nodiscard]] bool isJumpPoint(const sf::Vector2u& point) noexcept;

public: // TileNavpoint helpers
    void updateOpenSetWithCoord(NodeSet& openSet, const sf::Vector2u& coord, const NodeSet& closedSet, const sf::Vector2u& destinationCoord);

public:
    /**
     *  \brief Get path represented by tile indices to input mesh
     *
     *  All tiles along the path are returned, neighbouring tiles are always horizontal
     *  or vertical to each other, no diagonal transitions are allowed.
     */
    [[nodiscard]] dgm::Path<TileNavpoint> getPath(const sf::Vector2u& from, const sf::Vector2u& to);

    NavMesh() = delete;
    NavMesh(const dgm::Mesh& mesh);
    NavMesh(NavMesh&& other) = default;
    NavMesh(const NavMesh& other) = delete;
};

class WorldNavMesh {
protected:
    dgm::Mesh mesh;

    struct Connection {
        sf::Vector2u destination; ///< Destination node of the connection
        float distance; ///< Distance to destination

        Connection(const sf::Vector2u& destination, const float distance)
            : destination(destination), distance(distance) {}
    };
    
    /**
     *  \brief Map of connections between jump points
     *
     *  Map is indexed by jump point coordinates and each jump point lists set of
     *  jump points it is connected to, alonside with distance to each of them.
     *
     *  Jump points are only relevant for computation of WorldNavpoints
     */
    std::unordered_map<sf::Vector2u, std::vector<Connection>> jumpPointConnections = {};

protected:
    void discoverConnectionsForJumpPoint(const sf::Vector2u& point, bool fullSearch);

public:
    /**
     *  \brief Get path represented by world coordinates
     *
     *  The path only contains major "jump points" which can be far away from each other,
     *  where each two neighbouring jump points have unobstructed visibility to each other,
     *  and direction between them can be of any angle, not only horizontal and vertical.
     */
    [[nodiscard]] dgm::Path<WorldNavpoint> getPath(const sf::Vector2f& from, const sf::Vector2f& to);

    WorldNavMesh() = delete;
    WorldNavMesh(const dgm::Mesh& mesh);
    WorldNavMesh(WorldNavMesh&& other) = default;
    WorldNavMesh(const WorldNavMesh& other) = delete;
};

}
