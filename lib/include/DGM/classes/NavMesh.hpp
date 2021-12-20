#include <SFML/System/Vector2.hpp>
#include <DGM/classes/Path.hpp>
#include <DGM/classes/Objects.hpp>

#include <unordered_map>

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

namespace dgm {

/**
 *  \brief Class for computing path from collision mesh data
 */
class NavMesh {
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
    [[nodiscard]] bool isJumpPoint(const sf::Vector2u& point) noexcept;

    /**
     *  \brief Test if point has been previously discovered as jump point
     *
     *  \pre discoverJumpPoints has been called
     */
    /*[[nodiscard]] bool isJumpPoint(const sf::Vector2u& point) const noexcept {
        return jumpPointConnections.find(point) != jumpPointConnections.end();
    }*/

    /**
     *  \brief Mark two jump points as connected
     *
     *  Connections are symmetrical.
     */
    void connectTwoJumpPoints(const sf::Vector2u& a, const sf::Vector2u& b);

    void discoverJumpPoints();

    void discoverConnectionsForJumpPoint(const sf::Vector2u& point);

public:
    /**
     *  \brief Get path represented by world coordinates
     *
     *  The path only contains major "jump points" which can be far away from each other,
     *  where each two neighbouring jump points have unobstructed visibility to each other,
     *  and direction between them can be of any angle, not only horizontal and vertical.
     */
    [[nodiscard]] dgm::Path<WorldNavpoint> getPath(const sf::Vector2f& from, const sf::Vector2f& to);

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

}
