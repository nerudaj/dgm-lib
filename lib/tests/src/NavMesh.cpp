#include "TestDataDir.hpp"
#include <DGM/classes/Error.hpp>
#include <DGM/classes/NavMesh.hpp>
#include <DGM/classes/Utility.hpp>
#include <algorithm>
#include <catch2/catch_all.hpp>

#define NUMBER_DISTANCE(a, b) (std::max(a, b) - std::min(a, b))

#define REQUIRE_SAME_VECTORS(v1, v2)                                           \
    REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f);                            \
    REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

[[nodiscard]] dgm::Mesh buildMeshForTesting()
{
    const std::vector<bool> map = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    return dgm::Mesh(map, { 10u, 6u }, { 32, 32 });
}

class TestableNavMesh : public dgm::WorldNavMesh
{
public:
    [[nodiscard]] decltype(auto) getJumpPoints() const
    {
        std::vector<sf::Vector2u> result;

        std::transform(
            jumpPointConnections.begin(),
            jumpPointConnections.end(),
            std::back_inserter(result),
            [](const auto& itr) -> sf::Vector2u { return itr.first; });

        return result;
    }

    [[nodiscard]] bool
    arePointsConnected(const sf::Vector2u& a, const sf::Vector2u& b) const
    {
        auto& connections = jumpPointConnections.at(a);
        for (auto&& p : connections)
        {
            if (b == p.destination) return true;
        }
        return false;
    }

    TestableNavMesh(const dgm::Mesh& mesh) : dgm::WorldNavMesh(mesh) {}
};

TEST_CASE("Constructing WorldNavMesh", "[WorldNavMesh]")
{
    TestableNavMesh navmesh(buildMeshForTesting());

    SECTION("Jump point discovery")
    {
        auto points = navmesh.getJumpPoints();
        std::sort(
            points.begin(),
            points.end(),
            [](const sf::Vector2u& a, const sf::Vector2u& b) -> bool
            { return a.x <= b.x && a.y <= b.y; });

        REQUIRE(points.size() == 5);
        REQUIRE_SAME_VECTORS(points[0], sf::Vector2u(1, 2));
        REQUIRE_SAME_VECTORS(points[1], sf::Vector2u(3, 2));
        REQUIRE_SAME_VECTORS(points[2], sf::Vector2u(5, 2));
        REQUIRE_SAME_VECTORS(points[3], sf::Vector2u(1, 4));
        REQUIRE_SAME_VECTORS(points[4], sf::Vector2u(3, 4));
    }

    SECTION("Jump point connections")
    {
        CHECK(navmesh.arePointsConnected({ 1, 2 }, { 3, 2 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 2 }, { 5, 2 }));
        CHECK(navmesh.arePointsConnected({ 1, 2 }, { 1, 4 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 1, 2 }, { 3, 4 }));

        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 1, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 5, 2 }));
        CHECK(navmesh.arePointsConnected({ 3, 2 }, { 3, 4 }));
        CHECK_FALSE(navmesh.arePointsConnected({ 3, 2 }, { 1, 4 }));

        CHECK_FALSE(navmesh.arePointsConnected({ 5, 2 }, { 1, 2 }));
        CHECK(navmesh.arePointsConnected({ 5, 2 }, { 3, 2 }));
        CHECK(navmesh.arePointsConnected({ 5, 2 }, { 3, 4 }));
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

    SECTION("Computing path")
    {
        auto toWorldCoord = [](unsigned x, unsigned y)
        { return sf::Vector2f((x + 0.5f) * 32.f, (y + 0.5f) * 32.f); };

        auto toTileCoord = [](const sf::Vector2f& coord)
        {
            return sf::Vector2u(
                static_cast<unsigned>(coord.x) / 32u,
                static_cast<unsigned>(coord.y) / 32u);
        };

        REQUIRE_SAME_VECTORS(toWorldCoord(1, 2), sf::Vector2f(48.f, 80.f));
        REQUIRE_SAME_VECTORS(
            toTileCoord(sf::Vector2f(112.f, 48.f)), sf::Vector2u(3, 1));

        SECTION("From-To identity")
        {
            auto path =
                navmesh.computePath(toWorldCoord(1, 1), toWorldCoord(1, 1))
                    .value();
            REQUIRE(path.isTraversed());
        }

        SECTION("Direct path between from-to")
        {
            auto path =
                navmesh.computePath(toWorldCoord(1, 1), toWorldCoord(2, 1))
                    .value();
            REQUIRE_FALSE(path.isLooping());
            REQUIRE_FALSE(path.isTraversed());

            REQUIRE_SAME_VECTORS(
                toWorldCoord(2, 1), path.getCurrentPoint().coord);
            REQUIRE(path.getCurrentPoint().value == 0u);
            path.advance();
            REQUIRE(path.isTraversed());
        }

        SECTION("Normal path")
        {
            auto path =
                navmesh.computePath(toWorldCoord(5, 1), toWorldCoord(1, 3))
                    .value();
            REQUIRE_FALSE(path.isLooping());
            REQUIRE_FALSE(path.isTraversed());

            const std::vector<sf::Vector2u> refpoints = {
                { 3u, 2u },
                { 1u, 2u },
                { 1u, 3u },
            };

            std::cout << "Printing tile path:" << std::endl;
            for (auto&& point : refpoints)
            {
                auto&& coord = path.getCurrentPoint().coord;
                auto worldPoint = toWorldCoord(point.x, point.y);
                std::cout << dgm::Utility::to_string(coord) << " ("
                          << dgm::Utility::to_string(toTileCoord(coord)) << ")"
                          << std::endl;
                REQUIRE_SAME_VECTORS(worldPoint, coord);
                REQUIRE(path.getCurrentPoint().value == 0u);
                path.advance();
            }

            REQUIRE(path.isTraversed());
        }

        SECTION("No viable path")
        {
            REQUIRE_FALSE(
                navmesh.computePath(toWorldCoord(1, 1), toWorldCoord(8, 1)));
            REQUIRE_FALSE(
                navmesh.computePath(toWorldCoord(1, 1), toWorldCoord(6, 1)));
        }
    }
}

TEST_CASE("Computing Tile path", "[TileNavMesh]")
{
    const auto mesh = buildMeshForTesting();

    SECTION("Path exists, identity")
    {
        auto path = dgm::TileNavMesh::computePath(
                        sf::Vector2u(1, 1), sf::Vector2u(1, 1), mesh)
                        .value();
        REQUIRE(path.isTraversed());
    }

    SECTION("Path exists, non trivial")
    {
        auto path = dgm::TileNavMesh::computePath(
                        sf::Vector2u(1, 4), sf::Vector2u(5, 1), mesh)
                        .value();
        REQUIRE_FALSE(path.isLooping());
        REQUIRE(path.getLength() == 7u);

        const std::vector<sf::Vector2u> refpoints = {
            { 1u, 3u }, { 1u, 2u }, { 1u, 1u }, { 2u, 1u },
            { 3u, 1u }, { 4u, 1u }, { 5u, 1u },
        };

        std::cout << "Printing tile path:" << std::endl;
        for (auto&& point : refpoints)
        {
            std::cout << dgm::Utility::to_string(path.getCurrentPoint().coord)
                      << std::endl;
            REQUIRE_SAME_VECTORS(point, path.getCurrentPoint().coord);
            REQUIRE(path.getCurrentPoint().value == 0u);
            path.advance();
        }

        REQUIRE(path.isTraversed());
    }

    SECTION("Path does not exist")
    {
        REQUIRE_FALSE(dgm::TileNavMesh::computePath(
            sf::Vector2u(1, 1), sf::Vector2u(8, 1), mesh));
    }
}

TEST_CASE("[WorldNavMesh] - BUG: Crashing after several queries")
{
    std::vector<bool> blockMesh = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1,
        0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1,
        0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
        1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    dgm::Mesh mesh(blockMesh, { 15u, 15u }, { 64u, 64u });
    dgm::WorldNavMesh navmesh(mesh);

    REQUIRE(navmesh.computePath({ 100.f, 100.f }, { 288.f, 862.f }));
    REQUIRE(navmesh.computePath({ 288.f, 864.f }, { 796.f, 158.f }));
    REQUIRE(navmesh.computePath({ 800.f, 160.f }, { 542.f, 731.f }));
    REQUIRE(navmesh.computePath({ 544.f, 736.f }, { 676.f, 733.f }));
    REQUIRE(navmesh.computePath({ 672.f, 736.f }, { 545.f, 613.f }));

    /**
     *  NOTE: This bug was caused by the fact that some points in the repro
     *  were also the jump points.
     *
     *  Algorithm used to add some auxiliary connections from source and
     * destination points to the existing jump points, so it could compute A*
     * and then removed those auxiliary connections to return internal
     * structures to default state.
     *
     *  That is obviously not needed if source and/or destination are jump
     * points, because they're connections are already found (so no need to redo
     * this step) and they must not be deleted.
     */
}