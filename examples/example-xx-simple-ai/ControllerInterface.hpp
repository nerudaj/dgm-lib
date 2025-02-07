#pragma once

#include <limits>

enum InputCode : std::size_t
{
    Up = 0,
    Down,
    Left,
    Right,
    Max
};

class ControllerInterface
{
public:
    virtual bool goUp() const = 0;
    virtual bool goDown() const = 0;
    virtual bool goLeft() const = 0;
    virtual bool goRight() const = 0;
    virtual ~ControllerInterface() = default;
};
