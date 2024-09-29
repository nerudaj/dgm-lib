#pragma once

#include <DGM/dgm.hpp>
#include "Scene.hpp"
#include "ControllerInterface.hpp"

class Actor
{
protected:
    const float SPEED;
    ControllerInterface& input;
    sf::Color renderColor;
    dgm::Circle body;
    sf::Vector2f lookDir = { 1.f, 0.f };

public:
    Actor(float speed, sf::Color color, ControllerInterface& input)
        : SPEED(speed)
        , input(input)
        , renderColor(color)
    {
        body.setRadius(16.f);
    }

    void spawn(const sf::Vector2f& pos)
    {
        body.setPosition(pos);
    }

    void update(const dgm::Time& dt, const Scene& scene);

    void drawTo(dgm::Window& window)
    {
        body.debugRender(window, renderColor);
    }

    const dgm::Circle& getBody() const
    {
        return body;
    }

    const sf::Vector2f& getLookDir() const
    {
        return lookDir;
    }
};
