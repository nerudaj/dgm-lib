#include <catch.hpp>
#include <DGM/classes/NavMesh.hpp>
#include <DGM/classes/Utility.hpp>
#include <DGM/classes/Error.hpp>
#include <algorithm>

#define NUMBER_DISTANCE(a, b) \
	(std::max(a, b) - std::min(a, b))

#define REQUIRE_SAME_VECTORS(v1, v2) \
	REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f); \
	REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

[[nodiscard]] dgm::Mesh buildMeshForTesting() {
    const std::vector<int> map = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    LevelD::Mesh lvdmesh;
    lvdmesh.layerWidth = 10;
    lvdmesh.layerHeight = 6;
    lvdmesh.layers.push_back(LevelD::TileLayer{
        std::vector<uint16_t>(map.begin(), map.end()),
        std::vector<bool>(map.begin(), map.end())
    });
    return dgm::Mesh(lvdmesh);
}

class TestableNavMesh : public dgm::NavMesh {
public:
    [[nodiscard]] bool isJumpPoint_Public(const sf::Vector2u& point) noexcept {
        return isJumpPoint(point);
    }

    [[nodiscard]] decltype(auto) getJumpPoints() const {
        std::vector<sf::Vector2u> result;

        //using MapIteratorType = decltype(jumpPointConnections)::iterator;

        std::transform(jumpPointConnections.begin(), jumpPointConnections.end(), std::back_inserter(result), [] (const auto &itr) -> sf::Vector2u {
            return itr.first;
        });

        return result;
    }

    [[nodiscard]] const auto& getJumpPointConnections() const noexcept {
        return jumpPointConnections;
    }

    TestableNavMesh(const dgm::Mesh &mesh) : dgm::NavMesh(mesh) {}
};

TEST_CASE("IsJumpPoint", "[NavMesh]") {
    TestableNavMesh navmesh(buildMeshForTesting());

    SECTION("Valid jump points") {
        REQUIRE(navmesh.isJumpPoint_Public({ 1, 2 }));
        REQUIRE(navmesh.isJumpPoint_Public({ 1, 4 }));
        REQUIRE(navmesh.isJumpPoint_Public({ 3, 2 }));
        REQUIRE(navmesh.isJumpPoint_Public({ 3, 4 }));
        REQUIRE(navmesh.isJumpPoint_Public({ 5, 2 }));
    }

    SECTION("Invalid jump points") {
        REQUIRE(!navmesh.isJumpPoint_Public({ 1, 1 }));
        REQUIRE(!navmesh.isJumpPoint_Public({ 1, 3 }));
        REQUIRE(!navmesh.isJumpPoint_Public({ 2, 4 }));
        REQUIRE(!navmesh.isJumpPoint_Public({ 5, 1 }));
        REQUIRE(!navmesh.isJumpPoint_Public({ 6, 2 }));
    }
}

TEST_CASE("Constructing NavMesh", "[NavMesh]") {
    TestableNavMesh navmesh(buildMeshForTesting());

    SECTION("Jump point discovery") {
        auto points = navmesh.getJumpPoints();
        std::sort(points.begin(), points.end(), [] (const sf::Vector2u& a, const sf::Vector2u& b) -> bool {
            return a.x <= b.x && a.y <= b.y;
        });

        REQUIRE(points.size() == 5);
        REQUIRE_SAME_VECTORS(points[0], sf::Vector2u(1, 2));
        REQUIRE_SAME_VECTORS(points[1], sf::Vector2u(3, 2));
        REQUIRE_SAME_VECTORS(points[2], sf::Vector2u(5, 2));
        REQUIRE_SAME_VECTORS(points[3], sf::Vector2u(1, 4));
        REQUIRE_SAME_VECTORS(points[4], sf::Vector2u(3, 4));
    }

    SECTION("Jump point connections") {
        // TODO: this
    }
}

TEST_CASE("Computing Tile path", "[NavMesh]") {
    TestableNavMesh navmesh(buildMeshForTesting());

    SECTION("Path exists, identity") {
        auto path = navmesh.getPath(sf::Vector2u(1, 1), sf::Vector2u(1, 1));
        REQUIRE(path.isTraversed());
    }

    SECTION("Path exists, non trivial") {
        auto path = navmesh.getPath(sf::Vector2u(1, 4), sf::Vector2u(5, 1));

        while (!path.isTraversed()) {
            std::cout << dgm::Utility::to_string(path.getCurrentPoint().coord) << std::endl;
            path.advance();
        }
        // TODO: checks
    }

    SECTION("Path does not exist") {
        REQUIRE_THROWS_AS(navmesh.getPath(sf::Vector2u(1, 1), sf::Vector2u(8, 1)), dgm::GeneralException);
    }
}

TEST_CASE("Update open set with coord", "[NavMesh]") {
    dgm::NavMesh::NodeSet openSet, closedSet;
    closedSet.insertNode(dgm::NavMesh::Node(sf::Vector2u(1, 1), sf::Vector2u(2, 1), 0, dgm::NavMesh::Dir::Down));

    TestableNavMesh navmesh(buildMeshForTesting());
    navmesh.updateOpenSetWithCoord(openSet, sf::Vector2u(1, 1), closedSet, sf::Vector2u(2, 1));

    REQUIRE(!openSet.isEmpty());
    REQUIRE(!openSet.contains(sf::Vector2u(1, 0)));
    REQUIRE(!openSet.contains(sf::Vector2u(0, 1)));
    REQUIRE(openSet.contains(sf::Vector2u(1, 2)));
    REQUIRE(openSet.contains(sf::Vector2u(2, 1)));
}

bool operator<(const sf::Vector2u& a, const sf::Vector2u& b) {
    if (a.y < b.y) return true;
    else if (a.y == b.y) return a.x < b.x;
    return false;
}

TEST_CASE("Vector less", "[NavMesh]") {
    REQUIRE_FALSE(sf::Vector2u(1, 1) < sf::Vector2u(1, 1));

    REQUIRE(sf::Vector2u(1, 0) < sf::Vector2u(1, 1));
    REQUIRE_FALSE(sf::Vector2u(1, 1) < sf::Vector2u(1, 0));

    REQUIRE(sf::Vector2u(0, 1) < sf::Vector2u(1, 1));
    REQUIRE_FALSE(sf::Vector2u(1, 1) < sf::Vector2u(0, 1));

    REQUIRE(sf::Vector2u(2, 1) < sf::Vector2u(1, 2));
    REQUIRE_FALSE(sf::Vector2u(1, 2) < sf::Vector2u(2, 1));
}