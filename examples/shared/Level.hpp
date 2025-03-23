#pragma once

#include <DGM/dgm.hpp>

class [[nodiscard]] Level final
{
public:
    // Values of tileData are indices into textureClip
    // so each tile is associated with the correct texture
    // collisionData determine whether each tile is impassable (true)
    // or not (false).
    Level(
        const dgm::Mesh& tileData,
        const sf::Texture& texture,
        const dgm::Clip& textureClip,
        dgm::Mesh collisionData);

public:
    void draw(dgm::Window& window)
    {
        window.draw(tilemap);
    }

    constexpr const dgm::Mesh& getMesh() const noexcept
    {
        return mesh;
    }

    // void changeTileToVoid(unsigned x, unsigned y);

protected:
    dgm::Mesh mesh;
    dgm::TileMap tilemap;
};
