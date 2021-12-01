#include <SFML/System/Vector2.hpp>
#include <DGM/classes/Path.hpp>

namespace dgm {

/**
 *  \brief Class for computing path from collision mesh data
 */
class NavMesh {
protected:
    dgm::Mesh mesh;

    struct Connection {
        sf::Vector2u destination; ///< Destination node of the connectino
        float distance; ///< Distance to destination

        Connection() = delete;
        Connection(const sf::Vector2u& desctination, const float distance)
            : destination(destination), distance(distance) {}
        Connection(Connnetion&& other) = default;
        Connection(const Connection& other) = delete;
    };

    /**
     *  \brief Map of connections between jump points
     *
     *  Map is indexed by jump point coordinates and each jump point lists set of
     *  jump points it is connected to, alonside with distance to each of them.
     *
     *  Jump points are only relevant for computation of WorldNavpoints
     */
    std::unordered_map<sf::Vector2u, std::vector<Connection> jumpPointConnections = {};

protected:
    /**
     *  \brief Test if point has been previously discovered as jump point
     *
     *  \pre discoverJumpPoints has been called
     */
    [[nodiscard]] constexpr isJumpPoint(const sf::Vector2u& point) const noexcept {
        return jumpPointConnections.find(point) != jumpPointConnections.end();
    }
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
    [[nodiscard]] Path<WorldNavpoint> getPath(const sf::Vector2f& from, const sf::Vector2f& to);

    /**
     *  \brief Get path represented by tile indices to input mesh
     *
     *  All tiles along the path are returned, neighbouring tiles are always horizontal
     *  or vertical to each other, no diagonal transitions are allowed.
     */
    [[nodiscard]] Path<TileNavpoint> getPath(const sf::Vector2u& from, const sf::Vector2u& to);

    NavMesh() = delete;
    NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {}
    NavMesh(NavMesh&& other) = default;
    NavMesh(const NavMesh& other) = delete;
};

}

void dgm::NavMesh::connectTwoJumpPoints(const sf::Vector2u& a, const sf::Vector2u& b) {
    const float dx = (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
    const float dy = (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    jumpPointConnections[a].push_back(Connection(a, distance));
    jumpPointConnections[b].push_back(Connection(b, distance));
}

dgm::NavMesh::NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {
    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point);
}

void dgm::NavMesh::discoverJumpPoints() {
    auto isCornerPoint = [&mesh] (const sf::Vector2u &point) -> bool {
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
        const bool northWestCorner = mesh.at(point.x - 1, point.y - 1) > 0 && mesh.at(point.x - 1, point.y) <= 0 && mesh.at(point.x, point.y - 1);
        const bool northEastCorner = mesh.at(point.x + 1, point.y - 1) > 0 && mesh.at(point.x + 1, point.y) <= 0 && mesh.at(point.x, point.y - 1);
        const bool southWestCorner = mesh.at(point.x - 1, point.y + 1) > 0 && mesh.at(point.x - 1, point.y) <= 0 && mesh.at(point.x, point.y + 1);
        const bool southEastCorner = mesh.at(point.x + 1, point.y + 1) > 0 && mesh.at(point.x + 1, point.y) <= 0 && mesh.at(point.x, point.y + 1);

        return northWestCorner || northEastCorner || southWestCorner || southEastCorner;
    };

    for (unsigned y = 1; y < mesh.getDataSize().y - 1; y++) {
        for (unsigned x = 1; x < mesh.getDataSize().x - 1; x++) {
            // Skip impassable blocks
            if (mesh.at(x, y) > 0) continue;

            sf::Vector2u point(x, y);
            if (isCornerPoint(point)) {
                jumpPointConnections[point] = {};
            }
        }
    }
}

class TestableNavMesh : public dgm::NavMesh {
public:
    [[nodiscard]] decltype(auto) getJumpPoints() const {
        std::vector<sf::Vector2u> result;

        using MapIteratorType = decltype(jumpPointConnections)::iterator;

        std::tranform(jumpPointConnections.begin(), jumpPointConnections.end(), std::back_inserter(result), [] (MapIteratorType itr) -> sf::Vector2u {
            return itr->first;
        });

        return result;
    }

    [[nodiscard]] const decltype(auto)& getJumpPointConnections() const noexcept {
        return jumpPointConnections;
    }

    TestableNavMesh(const dgm::Mesh &mesh) : dgm::NavMesh(mesh) {}
};

TEST_CASE("Construction", "[NavMesh]") {
    /*
    {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
    */
    LevelD::Mesh lvdmesh; // TODO: init
    dgm::Mesh mesh(lvdmesh);
    TestableNavMesh navmesh(mesh);

    SECTION("Jump point discovery") {
        auto conns = navmesh.getJumpPoints();

        REQUIRE(conns.size() == 5);
        REQUIRE(COMPARE_VECTORS(conns[0], sf::Vector2u(5, 2)));
        REQUIRE(COMPARE_VECTORS(conns[1], sf::Vector2u(1, 2)));
        REQUIRE(COMPARE_VECTORS(conns[2], sf::Vector2u(3, 2)));
        REQUIRE(COMPARE_VECTORS(conns[3], sf::Vector2u(1, 4)));
        REQUIRE(COMPARE_VECTORS(conns[4], sf::Vector2u(3, 4)));
    }

    SECTION("Jump point connections") {

    }
}