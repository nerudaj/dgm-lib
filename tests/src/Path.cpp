#include "DGM/classes/Path.hpp"
#include "DGM/classes/NavMesh.hpp"
#include <catch2/catch_all.hpp>

namespace PathTests
{
    const std::vector<dgm::TileNavpoint> POINTS = { { { 0, 0 }, 10 },
                                                    { { 1, 0 }, 0 },
                                                    { { 2, 0 }, 5 } };

    TEST_CASE("Construction", "Path")
    {
        SECTION("From vector")
        {
            REQUIRE_NOTHROW(dgm::Path(POINTS, true));
            REQUIRE_NOTHROW(dgm::Path(dgm::Path(POINTS, true)));
        }

        SECTION("Move assigment")
        {
            dgm::WorldNavMesh navmesh(dgm::Mesh({ 1u, 1u }, { 1u, 1u }));
            auto destinationPath =
                navmesh.computePath({ 0.f, 0.f }, { 0.f, 0.f });
            auto path2 = std::move(destinationPath);
        }

        SECTION("Can be cloned")
        {
            auto path1 = dgm::Path(POINTS, true);
            path1.advance();
            auto path2 = path1.clone();
            REQUIRE(
                path1.getCurrentPoint().coord == path2.getCurrentPoint().coord);
        }
    }

    TEST_CASE("Traversal", "Path")
    {
        SECTION("Non-looping")
        {
            dgm::Path path(POINTS, false);

            REQUIRE(not path.isLooping());
            REQUIRE(not path.isTraversed());
            path.advance();
            REQUIRE(not path.isTraversed());
            path.advance();
            REQUIRE(not path.isTraversed());
            path.advance();
            REQUIRE(path.isTraversed());
        }

        SECTION("Looping")
        {
            dgm::Path path(POINTS, true);
            REQUIRE(path.getLength() == 3u);
            REQUIRE(path.isLooping());
            REQUIRE(not path.isTraversed());
            path.advance();
            REQUIRE(not path.isTraversed());
            path.advance();
            REQUIRE(not path.isTraversed());
            REQUIRE(path.getCurrentPoint().coord.x == 2ul);
            REQUIRE(path.getCurrentPoint().coord.y == 0ul);
            path.advance();
            REQUIRE(not path.isTraversed());
        }
    }

} // namespace PathTests
