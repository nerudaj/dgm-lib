#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Error.hpp>
#include <cassert>
#include <fstream>

using dgm::Animation;
using dgm::AnimationStates;

/**
 *  WARNING: noexcept relies heavily on the fact that sf::Time is just a wrapper
 *  around std::chrono and that chrono is noexcept.
 */

Animation::PlaybackStatus Animation::update(const dgm::Time& time) noexcept
{
    static_assert(noexcept(time.getElapsed()));
    static_assert(noexcept(hasClipFinishedPlaying()));
    static_assert(noexcept(isLooping()));

    if (hasClipFinishedPlaying())
    {
        return PlaybackStatus::Finished;
    }

    elapsedTime += time.getElapsed();
    currentFrameIndex = static_cast<unsigned>(elapsedTime / timePerFrame);

    if (hasClipFinishedPlaying())
    {
        if (not isLooping())
        {
            return PlaybackStatus::Finished;
        }

        currentFrameIndex = 0;

        auto allFramesLength =
            timePerFrame
            * static_cast<float>(currentState->second.getFrameCount());

        while (elapsedTime > allFramesLength)
            elapsedTime -= allFramesLength;
    }

    return PlaybackStatus::Playing;
}

void Animation::setState(const std::string& stateName, bool shouldLoop)
{
    if (getStateName() == stateName && isLooping() == shouldLoop) return;

    auto newState = states.find(stateName);
    if (newState == states.end())
    {
        throw dgm::Exception("Cannot find animation state '" + stateName + "'");
    }

    currentState = newState;
    setLooping(shouldLoop);
    reset();
}

void Animation::setSpeed(unsigned framesPerSecond) noexcept
{
    static_assert(noexcept(std::round(0.f)));

    assert(framesPerSecond > 0);
    timePerFrame = sf::milliseconds(
        static_cast<std::int32_t>(std::round(1000.f / framesPerSecond)));
}

Animation::Animation(
    const AnimationStates& _states, int framesPerSecond) noexcept
    : states(_states), currentState(states.begin())
{
    static_assert(noexcept(states.begin()));
    static_assert(noexcept(setSpeed(framesPerSecond)));

    setSpeed(framesPerSecond);
}
