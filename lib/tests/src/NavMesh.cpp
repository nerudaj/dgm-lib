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
    const std::vector<uint16_t> map = {
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
    lvdmesh.tileWidth = 32;
    lvdmesh.tileHeight = 32;
    lvdmesh.layers.push_back(LevelD::TileLayer{
        std::vector<uint16_t>(map.begin(), map.end()),
        std::vector<bool>(map.begin(), map.end())
    });
    return dgm::Mesh(lvdmesh);
}

class TestableNavMesh : public dgm::WorldNavMesh {
public:
    [[nodiscard]] decltype(auto) getJumpPoints() const {
        std::vector<sf::Vector2u> result;

        std::transform(jumpPointConnections.begin(), jumpPointConnections.end(), std::back_inserter(result), [] (const auto &itr) -> sf::Vector2u {
            return itr.first;
        });

        return result;
    }

    [[nodiscard]] bool arePointsConnected(const sf::Vector2u& a, const sf::Vector2u& b) const {
        auto& connections = jumpPointConnections.at(a);
        for (auto&& p : connections) {
            if (b == p.destination) return true;
        }
        return false;
    }

    TestableNavMesh(const dgm::Mesh &mesh) : dgm::WorldNavMesh(mesh) {}
};

TEST_CASE("Constructing TileNavMesh", "[TileNavMesh]") {
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
        CHECK(navmesh.arePointsConnected({ 1, 2 }, { 3, 2 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 2 }, { 5, 2 }));
        CHECK(navmesh.arePointsConnected({ 1, 2 }, { 1, 4 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 2 }, { 3, 4 }));

        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 1, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 5, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 3, 4 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 3, 2 }, { 1, 4 }));

        CHECK_FALSE(navmesh.arePointsConnected({5, 2}, {1, 2}));
        CHECK(navmesh.arePointsConnected({5, 2}, {3, 2}));
        CHECK(navmesh.arePointsConnected({5, 2}, {3, 4}));
        CHECK_FALSE(navmesh.arePointsConnected({ 5, 2 }, { 1, 4 }));

        CHECK(navmesh.arePointsConnected({ 1, 4 }, { 1, 2 }));
        CHECK(navmesh.arePointsConnected({ 1, 4 }, { 3, 4 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 4 }, { 3, 2 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 4 }, { 5, 2 }));

        CHECK_FALSE(navmesh.arePointsConnected({ 3, 4 }, { 1, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 4 }, { 3, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 4 }, { 5, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 4 }, { 1, 4 }));
    }

    SECTION("Computing path") {
        auto toWorldCoord = [] (unsigned x, unsigned y) {
            return sf::Vector2f((x + 0.5f) * 32.f, (y + 0.5f) * 32.f);
        };

        auto toTileCoord = [] (const sf::Vector2f& coord) {
            return sf::Vector2u(static_cast<unsigned>(coord.x) / 32u, static_cast<unsigned>(coord.y) / 32u);
        };

        REQUIRE_SAME_VECTORS(toWorldCoord(1, 2), sf::Vector2f(48.f, 80.f));
        REQUIRE_SAME_VECTORS(toTileCoord(sf::Vector2f(112.f, 48.f)), sf::Vector2u(3, 1));

        SECTION("From-To identity") {
            auto path = navmesh.getPath(toWorldCoord(1, 1), toWorldCoord(1, 1));
            REQUIRE(path.isTraversed());
        }

        SECTION("Direct path between from-to") {
            auto path = navmesh.getPath(toWorldCoord(1, 1), toWorldCoord(2, 1));
            REQUIRE_FALSE(path.isLooping());
            REQUIRE_FALSE(path.isTraversed());

            REQUIRE_SAME_VECTORS(toWorldCoord(2, 1), path.getCurrentPoint().coord);
            REQUIRE(path.getCurrentPoint().value == 0u);
            path.advance();
            REQUIRE(path.isTraversed());
        }

        SECTION("Normal path") {
            auto path = navmesh.getPath(toWorldCoord(5, 1), toWorldCoord(1, 3));
            REQUIRE_FALSE(path.isLooping());
            REQUIRE_FALSE(path.isTraversed());

            const std::vector<sf::Vector2u> refpoints = {
                {3u, 2u},
                {1u, 2u},
                {1u, 3u},
            };

            std::cout << "Printing tile path:" << std::endl;
            for (auto&& point : refpoints) {
                auto&& coord = path.getCurrentPoint().coord;
                auto worldPoint = toWorldCoord(point.x, point.y);
                std::cout << dgm::Utility::to_string(coord) << " (" << dgm::Utility::to_string(toTileCoord(coord)) << ")" << std::endl;
                REQUIRE_SAME_VECTORS(worldPoint, coord);
                REQUIRE(path.getCurrentPoint().value == 0u);
                path.advance();
            }

            REQUIRE(path.isTraversed());
        }

        SECTION("No viable path") {
            // There is no path because each point lies in a separate section of the map
            REQUIRE_THROWS_AS(navmesh.getPath(toWorldCoord(1, 1), toWorldCoord(8, 1)), dgm::GeneralException);

            // There is no path because destination point is a wall
            REQUIRE_THROWS_AS(navmesh.getPath(toWorldCoord(1, 1), toWorldCoord(6, 1)), dgm::GeneralException);
        }
    }
}

TEST_CASE("Computing Tile path", "[TileNavMesh]") {
    const auto mesh = buildMeshForTesting();

    SECTION("Path exists, identity") {
        auto path = dgm::TileNavMesh::getPath(sf::Vector2u(1, 1), sf::Vector2u(1, 1), mesh);
        REQUIRE(path.isTraversed());
    }

    SECTION("Path exists, non trivial") {
        auto path = dgm::TileNavMesh::getPath(sf::Vector2u(1, 4), sf::Vector2u(5, 1), mesh);
        REQUIRE_FALSE(path.isLooping());

        const std::vector<sf::Vector2u> refpoints = {
            {1u, 3u},
            {1u, 2u},
            {1u, 1u},
            {2u, 1u},
            {3u, 1u},
            {4u, 1u},
            {5u, 1u},
        };

        std::cout << "Printing tile path:" << std::endl;
        for (auto&& point : refpoints) {
            std::cout << dgm::Utility::to_string(path.getCurrentPoint().coord) << std::endl;
            REQUIRE_SAME_VECTORS(point, path.getCurrentPoint().coord);
            REQUIRE(path.getCurrentPoint().value == 0u);
            path.advance();
        }

        REQUIRE(path.isTraversed());
    }

    SECTION("Path does not exist") {
        REQUIRE_THROWS_AS(dgm::TileNavMesh::getPath(sf::Vector2u(1, 1), sf::Vector2u(8, 1), mesh), dgm::GeneralException);
    }
}
