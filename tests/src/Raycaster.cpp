#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>
#include <vector>

namespace
{
    // 10x8 grid with border walls and one interior wall at (4,3)
    //
    // Row 0: 1 1 1 1 1 1 1 1 1 1
    // Row 1: 1 . . . . . . . . 1
    // Row 2: 1 . . . . . . . . 1
    // Row 3: 1 . . . W . . . . 1
    // Row 4: 1 . . . . . . . . 1
    // Row 5: 1 . . . . . . . . 1
    // Row 6: 1 . . . . . . . . 1
    // Row 7: 1 1 1 1 1 1 1 1 1 1
    [[nodiscard]] dgm::Mesh buildTestMesh()
    {
        // clang-format off
        const std::vector<int> map = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        };
        // clang-format on

        return dgm::Mesh(map, { 10u, 8u }, { 32u, 32u });
    }

    constexpr float VOXEL = 32.f;

    // Center of tile (x, y) in world coordinates
    [[nodiscard]] sf::Vector2f tileCenter(unsigned x, unsigned y)
    {
        return { (x + 0.5f) * VOXEL, (y + 0.5f) * VOXEL };
    }
} // namespace

TEST_CASE("[Raycaster] - hasDirectVisibility")
{
    auto mesh = buildTestMesh();

    SECTION("Clear horizontal line of sight")
    {
        // Row 1 is entirely open between border walls
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(1, 1), tileCenter(8, 1), mesh));
    }

    SECTION("Clear vertical line of sight")
    {
        // Column x=1 is entirely open between border walls
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(1, 1), tileCenter(1, 6), mesh));
    }

    SECTION("Clear diagonal line of sight")
    {
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(1, 1), tileCenter(3, 2), mesh));
    }

    SECTION("Blocked horizontal line of sight by interior wall")
    {
        // Row 3 has a wall at x=4, blocking (1,3) -> (8,3)
        REQUIRE_FALSE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(1, 3), tileCenter(8, 3), mesh));
    }

    SECTION("Blocked diagonal line of sight through interior wall")
    {
        // Ray from (2,1) to (6,5) passes through (4,3) wall
        REQUIRE_FALSE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(2, 1), tileCenter(6, 5), mesh));
    }

    SECTION("Visibility between adjacent tiles")
    {
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(1, 1), tileCenter(2, 1), mesh));
    }

    SECTION("Clear line of sight above obstacle")
    {
        // From (3,2) to (5,2) passes above the wall at (4,3)
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(
                tileCenter(3, 2), tileCenter(5, 2), mesh));
    }

    SECTION("Visibility is symmetric")
    {
        const auto a = tileCenter(1, 3);
        const auto b = tileCenter(8, 3);
        REQUIRE(
            dgm::Raycaster::hasDirectVisibility(a, b, mesh)
            == dgm::Raycaster::hasDirectVisibility(b, a, mesh));
    }
}

TEST_CASE("[Raycaster] - raycast")
{
    auto mesh = buildTestMesh();

    SECTION("Eastward ray hits east border wall")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(1, 1), { 1.f, 0.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(9u, 1u));
        REQUIRE(result.hitDirection == dgm::Raycaster::HitDirection::Vertical);
    }

    SECTION("Westward ray hits west border wall")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(8, 4), { -1.f, 0.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(0u, 4u));
        REQUIRE(result.hitDirection == dgm::Raycaster::HitDirection::Vertical);
    }

    SECTION("Southward ray hits south border wall")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(1, 1), { 0.f, 1.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(1u, 7u));
        REQUIRE(
            result.hitDirection == dgm::Raycaster::HitDirection::Horizontal);
    }

    SECTION("Northward ray hits north border wall")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(5, 6), { 0.f, -1.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(5u, 0u));
        REQUIRE(
            result.hitDirection == dgm::Raycaster::HitDirection::Horizontal);
    }

    SECTION("Eastward ray hits interior wall")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(1, 3), { 1.f, 0.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(4u, 3u));
        REQUIRE(result.hitDirection == dgm::Raycaster::HitDirection::Vertical);
    }

    SECTION("Diagonal ray hits interior wall")
    {
        // Direction (3,2) from tile (1,1) — DDA lands on (4,3)
        auto result =
            dgm::Raycaster::raycast(tileCenter(1, 1), { 3.f, 2.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(4u, 3u));
        REQUIRE(result.hitDirection == dgm::Raycaster::HitDirection::Vertical);
    }

    SECTION("Ray originating from inside a solid tile returns that tile")
    {
        auto result =
            dgm::Raycaster::raycast(tileCenter(4, 3), { 1.f, 0.f }, mesh);

        REQUIRE(result.tile == sf::Vector2u(4u, 3u));
    }
}

TEST_CASE("[Raycaster] - raycast with callback")
{
    auto mesh = buildTestMesh();

    SECTION("Callback records all traversed empty tiles")
    {
        std::vector<sf::Vector2u> visited;
        auto result = dgm::Raycaster::raycast(
            tileCenter(1, 3),
            { 1.f, 0.f },
            mesh,
            [&visited](const sf::Vector2u& tile) { visited.push_back(tile); });

        REQUIRE(result.tile == sf::Vector2u(4u, 3u));
        REQUIRE(visited.size() == 3);
        CHECK(visited[0] == sf::Vector2u(1u, 3u));
        CHECK(visited[1] == sf::Vector2u(2u, 3u));
        CHECK(visited[2] == sf::Vector2u(3u, 3u));
    }

    SECTION("Callback is not invoked when origin is inside a solid tile")
    {
        std::vector<sf::Vector2u> visited;
        auto result = dgm::Raycaster::raycast(
            tileCenter(4, 3),
            { 1.f, 0.f },
            mesh,
            [&visited](const sf::Vector2u& tile) { visited.push_back(tile); });

        REQUIRE(result.tile == sf::Vector2u(4u, 3u));
        REQUIRE(visited.empty());
    }

    SECTION("Callback receives tiles in traversal order along a long ray")
    {
        std::vector<sf::Vector2u> visited;
        // Southward from (1,1) — traverses tiles (1,1)..(1,6), hits (1,7)
        auto result = dgm::Raycaster::raycast(
            tileCenter(1, 1),
            { 0.f, 1.f },
            mesh,
            [&visited](const sf::Vector2u& tile) { visited.push_back(tile); });

        REQUIRE(result.tile == sf::Vector2u(1u, 7u));
        REQUIRE(visited.size() == 6);
        for (unsigned i = 0; i < 6; ++i)
        {
            CHECK(visited[i] == sf::Vector2u(1u, i + 1u));
        }
    }
}
