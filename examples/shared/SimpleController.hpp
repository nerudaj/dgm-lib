#pragma once

#include <DGM/dgm.hpp>

class SimpleController
{
public:
    enum class Action
    {
        Up,
        Down,
        Left,
        Right,
    };

public:
    SimpleController();

public:
    sf::Vector2f getForward() const;

private:
    dgm::Controller<Action> input;
};
