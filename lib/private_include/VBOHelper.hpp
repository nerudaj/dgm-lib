#pragma once

class VBOHelper
{
public:
    static void setTextureCoords(sf::Vertex* quad, const sf::IntRect& frame)
    {
        quad[0].texCoords = sf::Vector2f(frame.position);
        quad[1].texCoords =
            sf::Vector2f(frame.position + sf::Vector2i { frame.size.x, 0 });
        quad[2].texCoords =
            sf::Vector2f(frame.position + sf::Vector2i { 0, frame.size.y });

        quad[3].texCoords = quad[1].texCoords;
        quad[4].texCoords = sf::Vector2f(frame.position + frame.size);
        quad[5].texCoords = quad[2].texCoords;
    }
};