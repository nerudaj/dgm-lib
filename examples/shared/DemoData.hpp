#pragma once

#include "Level.hpp"
#include <DGM/classes/Clip.hpp>
#include <SFML/Graphics/Texture.hpp>

class DemoData
{
public:
    static Level
    createDemoLevel(const sf::Texture& texture, const dgm::Clip& clip);

    static Level
    createDemoLevel2(const sf::Texture& texture, const dgm::Clip& clip);

    static dgm::Clip getClipForExampleTileset()
    {
        return dgm::Clip(sf::Vector2u(32u, 32u), sf::IntRect(0, 0, 64, 64));
    }
};