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
        dgm::Path<dgm::WorldNavpoint> destinationPath;

        SECTION("Empty ctor")
        {
            auto path = dgm::Path<dgm::WorldNavpoint>();
        }

        SECTION("From vector")
        {
            REQUIRE_NOTHROW(dgm::Path(POINTS, true));
            REQUIRE_NOTHROW(dgm::Path(dgm::Path(POINTS, true)));
        }

        SECTION("Move assigment")
        {
            dgm::Mesh mesh;
            mesh.setVoxelSize(1u, 1u);
            mesh.setDataSize(1u, 1u);
            dgm::WorldNavMesh navmesh(mesh);
            destinationPath = navmesh.getPath({ 0.f, 0.f }, { 0.f, 0.f });
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
