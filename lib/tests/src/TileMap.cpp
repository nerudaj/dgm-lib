#include <DGM/classes/TileMap.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[TileMap]")
{
    SECTION("Default constructor works")
    {
        REQUIRE_NOTHROW([]() { dgm::TileMap tilemap; }());
    }

    SECTION("Proper constructor works")
    {
        sf::Texture texture;
        dgm::Clip clip;

        dgm::TileMap tilemap = dgm::TileMap(texture, std::move(clip));
    }
}