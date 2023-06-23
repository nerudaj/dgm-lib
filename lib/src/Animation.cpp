#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Error.hpp>
#include <cassert>
#include <fstream>

using dgm::Animation;
using dgm::AnimationStates;

static inline AnimationStates NullStates = {};

bool Animation::update(const dgm::Time& time)
{
    elapsedTime += time.getElapsed();
    if (elapsedTime > timePerFrame)
    {
        elapsedTime -= timePerFrame;

        currentFrameIndex++;
        if (currentFrameIndex == currentState->second.getFrameCount())
        {
            currentFrameIndex = 0;
            if (not isLooping())
            {
                return false;
            }
        }
    }

    return true;
}

void Animation::setState(const std::string& state, bool shouldLoop)
{
    if (isCurrentStateValid() && currentState->first == state) return;

    auto newState = states.get().find(state);
    if (newState == states.get().end())
    {
        throw dgm::Exception("Cannot find animation state '" + state + "'");
    }

    currentState = newState;
    setLooping(shouldLoop);

    reset();
}

void Animation::setSpeed(unsigned framesPerSecond)
{
    assert(framesPerSecond > 0);
    timePerFrame = sf::milliseconds(1000 / framesPerSecond);
}

Animation::Animation() : Animation(NullStates) {}

Animation::Animation(const AnimationStates& inStates, int framesPerSecond)
    : states(cref(inStates))
{
    elapsedTime = sf::seconds(0);

    setSpeed(framesPerSecond);
    currentFrameIndex = 0;
    currentState = states.get().end();

    setLooping(false);
}
