#include <catch.hpp>
#include <DGM/classes/NavMesh.hpp>

#include <algorithm>

#define NUMBER_DISTANCE(a, b) \
	(std::max(a, b) - std::min(a, b))

#define REQUIRE_SAME_VECTORS(v1, v2) \
	REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f); \
	REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

class TestableNavMesh : public dgm::NavMesh {
public:
    [[nodiscard]] decltype(auto) getJumpPoints() const {
        std::vector<sf::Vector2u> result;

        using MapIteratorType = decltype(jumpPointConnections)::iterator;

        std::transform(jumpPointConnections.begin(), jumpPointConnections.end(), std::back_inserter(result), [] (MapIteratorType itr) -> sf::Vector2u {
            return itr->first;
        });

        return result;
    }

    [[nodiscard]] const auto& getJumpPointConnections() const noexcept {
        return jumpPointConnections;
    }

    TestableNavMesh(const dgm::Mesh &mesh) : dgm::NavMesh(mesh) {}
};

TEST_CASE("Construction", "[NavMesh]") {
    const std::vector<int> map = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };
    
    LevelD::Mesh lvdmesh;
    lvdmesh.layerWidth = 8;
    lvdmesh.layerHeight = 6;
    lvdmesh.layers.push_back(LevelD::TileLayer{
        std::vector<uint16_t>(map.begin(), map.end()),
        std::vector<bool>(map.begin(), map.end())
    });
    dgm::Mesh mesh(lvdmesh);
    TestableNavMesh navmesh(mesh);

    SECTION("Jump point discovery") {
        auto conns = navmesh.getJumpPoints();

        REQUIRE(conns.size() == 5);
        REQUIRE_SAME_VECTORS(conns[0], sf::Vector2u(5, 2));
        REQUIRE_SAME_VECTORS(conns[1], sf::Vector2u(1, 2));
        REQUIRE_SAME_VECTORS(conns[2], sf::Vector2u(3, 2));
        REQUIRE_SAME_VECTORS(conns[3], sf::Vector2u(1, 4));
        REQUIRE_SAME_VECTORS(conns[4], sf::Vector2u(3, 4));
    }

    SECTION("Jump point connections") {

    }
}
