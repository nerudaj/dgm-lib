#pragma once

#include <DGM/dgm.hpp>

class SimpleController
{
public:
    enum Action
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
    dgm::Controller input;
};
