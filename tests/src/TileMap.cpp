#include <DGM/classes/TileMap.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[TileMap]")
{
    SECTION("Proper constructor works")
    {
        sf::Texture texture;
        auto&& clip = dgm::Clip({ 1, 1 }, { 0, 0, 1, 1 });

        dgm::TileMap tilemap = dgm::TileMap(texture, std::move(clip));
    }
}