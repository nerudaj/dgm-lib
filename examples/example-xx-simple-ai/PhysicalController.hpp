#pragma once

#include <DGM/classes/Controller.hpp>
#include "ControllerInterface.hpp"

class PhysicalController : public ControllerInterface
{
protected:
    dgm::Controller input;

public:
    PhysicalController()
    {
        input.bindInput(InputCode::Up, sf::Keyboard::W);
        input.bindInput(InputCode::Down, sf::Keyboard::S);
        input.bindInput(InputCode::Left, sf::Keyboard::A);
        input.bindInput(InputCode::Right, sf::Keyboard::D);
    }

    bool goUp() const override
    {
        return input.getValue(InputCode::Up) > 0.f;
    }
    bool goDown() const override
    {
        return input.getValue(InputCode::Down) > 0.f;
    }
    bool goLeft() const override
    {
        return input.getValue(InputCode::Left) > 0.f;
    }
    bool goRight() const override
    {
        return input.getValue(InputCode::Right) > 0.f;
    }
};
