#pragma once

#include <array>
#include "ControllerInterface.hpp"

class NpcController : public ControllerInterface
{
protected:
    std::array<bool, InputCode::Max> inputs = { false, false, false, false };

public:
    bool goUp() const override
    {
        return inputs[InputCode::Up];
    }
    bool goDown() const override
    {
        return inputs[InputCode::Down];
    }
    bool goLeft() const override
    {
        return inputs[InputCode::Left];
    }
    bool goRight() const override
    {
        return inputs[InputCode::Right];
    }

    void releaseAllInputs()
    {
        for (auto& input : inputs)
            input = false;
    }

    void setInput(InputCode code)
    {
        inputs[code] = true;
    }
};
