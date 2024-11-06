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

    static dgm::ResourceManager loadDemoResources();
};