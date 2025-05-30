#include <DGM/classes/TileMap.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <VBOHelper.hpp>
#include <cassert>

void dgm::TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = texturePtr;

    // draw the vertex array
    target.draw(vertices, states);
}

void dgm::TileMap::changeTile(
    float x, float y, uint32_t tileIndex, uint32_t tileValue)
{
    assert(tileValue < clip.getFrameCount());
    assert(tileIndex * size_t(6) < vertices.getVertexCount());

    // Find textureRect for this tile
    const sf::IntRect& frame = clip.getFrame(tileValue);

    // get a pointer to the current tile's quad
    sf::Vertex* quad = &vertices[tileIndex * size_t(6)];

    // Define corners
    quad[0].position = sf::Vector2f(x * tileSize.x, y * tileSize.y);
    quad[1].position = sf::Vector2f((x + 1.f) * tileSize.x, y * tileSize.y);
    quad[2].position = sf::Vector2f(x * tileSize.x, (y + 1.f) * tileSize.y);

    quad[3].position = quad[1].position;
    quad[4].position =
        sf::Vector2f((x + 1.f) * tileSize.x, (y + 1.f) * tileSize.y);
    quad[5].position = quad[2].position;

    VBOHelper::setTextureCoords(quad, frame);
}

void dgm::TileMap::build(
    const sf::Vector2u newTileSize,
    const std::vector<int>& imageData,
    const sf::Vector2u& newDataSize)
{
    if (!texturePtr)
    {
        throw dgm::Exception(
            "You have to call dgm::TileMap::init prior to "
            "dgm::TileMap::build!");
    }

    assert(imageData.size() == size_t(newDataSize.x) * newDataSize.y);

    // TileMap::clip = clip;
    tileSize = sf::Vector2f(float(newTileSize.x), float(newTileSize.y));
    dataSize = newDataSize;

    vertices = sf::VertexArray(
        sf::PrimitiveType::Triangles, size_t(6) * dataSize.x * dataSize.y);

    // Loop over all tiles
    for (size_t y = 0; y < dataSize.y; y++)
    {
        for (size_t x = 0; x < dataSize.x; x++)
        {
            changeTile(
                static_cast<uint32_t>(x),
                static_cast<uint32_t>(y),
                imageData[y * dataSize.x + x]);
        }
    }
}
