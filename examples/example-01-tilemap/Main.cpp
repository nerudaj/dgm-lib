/*
An example showing a built-in class for rendering 2D tilemaps.
Implementation of dgm::TileMap is based on SFML tutorials:
https://www.sfml-dev.org/tutorials/2.6/graphics-vertex-array.php

TileMap is more efficient to render than to render tiles individually.
*/

#include "DemoData.hpp"
#include <DGM/dgm.hpp>

dgm::TileMap createSampleTilemap(const dgm::ResourceManager& resmgr)
{
    auto&& tilemap = dgm::TileMap(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));

    // clang-format off
    tilemap.build(
        {64u, 64u}, // size of each tile in pixels
        { // vector of indices into dgm::Clip given to the dgm::TileMap
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 2, 2, 1, 
            1, 0, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        },
        {15, 10}); // 15x10 tiles
    // clang-format on

    // NOTE: Once constructed, a texture/clip is bound to a tilemap
    // but tilemap can be rebuilt many times.
    // You can even call changeTile to alter individual tiles (slow).

    return tilemap;
}

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Tilemap", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& tilemap = createSampleTilemap(resmgr);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
        }

        /* DRAW */
        window.beginDraw();

        window.draw(tilemap); // draws like a native sf primitive

        window.endDraw();
    }

    return 0;
}