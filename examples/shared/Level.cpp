#include "Level.hpp"

Level::Level(
    const dgm::Mesh& tileData,
    const sf::Texture& texture,
    const dgm::Clip& textureClip,
    const dgm::Mesh& collisionData)
    : tilemap(texture, textureClip), mesh(collisionData)
{
    // TileMap is initialized with a particular texture and clipping
    // but can be rebuilt from ground up any amount of times
    tilemap.build(
        tileData.getVoxelSize(),
        tileData.getRawConstData(),
        tileData.getDataSize());

    // also individual tiles can be changed with changeTile
}

/*
void Level::changeTileToVoid(unsigned x, unsigned y)
{
    tilemap.changeTile(x, y, 0);
    mesh[y * mesh.getDataSize().x + x] = 0;
}
*/
