#pragma once

#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Compatibility.hpp>
#include <DGM/classes/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <functional>
#include <map>
#include <memory>

namespace dgm
{
    typedef std::map<std::string, dgm::Clip> AnimationStates;

    /**
     *  \brief Object animating a single sprite
     *
     *  \details Animation contains a database of animation states
     *  and a pointer to some sprite. When you set a certain named state
     *  and start calling update, then the image displayed on sprite
     *  will be changed.
     */
    class [[nodiscard]] Animation final
    {
    public:
        enum class PlaybackStatus // not nodiscard on purpose
        {
            Playing,
            Finished
        };

    public:
        /**
         *  \brief Construct animation object from states and required FPS
         *
         *  The states are not copied! They must outlive the animation object.
         */
        Animation(
            const AnimationStates& states, int framesPerSecond = 30) noexcept;

    public:
        /**
         *  \brief Update animation object with time
         *
         *  \details If time progressed by enough amount (defined by \ref
         * setSpeed) then boundSprite is updated to show next frame of the
         * animation. If no more frames are available for currently set state
         * and looping is set to false, then update will return false. Otherwise
         * it returns true.
         */
        PlaybackStatus update(const dgm::Time& time) noexcept;

        /**
         *  \brief Set the new animation state
         *
         *  \details Animation will play from the first
         *  frame of the new animation state. If looping is set
         *  to true then animation will start over when it finishes
         *  and \ref update will always return true.
         *
         * If getStateName() equals stateName and isLooping() equals looping,
         * this function does nothing.
         *
         * If stateName is not present in states map, the function throws.
         */
        void setState(const std::string& stateName, bool looping = false);

        /**
         *  \brief Set speed of animation in frames per second
         */
        void setSpeed(unsigned framesPerSecond) noexcept;

        NODISCARD_RESULT bool hasClipFinishedPlaying() const noexcept
        {
            return currentFrameIndex >= currentState->second.getFrameCount();
        }

        constexpr void setLooping(bool enabled) noexcept
        {
            looping = enabled;
        }

        /**
         *  \brief Get speed as number of frames per second
         */
        NODISCARD_RESULT unsigned getSpeed() const noexcept
        {
            return static_cast<unsigned>(
                std::round(1000.f / timePerFrame.asMilliseconds()));
        }

        /**
         *  \brief Get name of currently selected state
         */
        NODISCARD_RESULT const std::string& getStateName() const noexcept
        {
            return currentState->first;
        }

        NODISCARD_RESULT constexpr bool isLooping() const noexcept
        {
            return looping;
        }

        NODISCARD_RESULT const sf::IntRect& getCurrentFrame() const noexcept
        {
            return currentState->second.getFrame(currentFrameIndex);
        }

        /**
         *  \brief Reset the current state of animation to frame 0
         */
        constexpr void reset() noexcept
        {
            currentFrameIndex = 0;
            elapsedTime = sf::Time::Zero;
        }

    private:
        const AnimationStates& states;
        AnimationStates::const_iterator currentState;
        sf::Time elapsedTime = sf::seconds(0);
        sf::Time timePerFrame = sf::seconds(0);
        std::size_t currentFrameIndex = 0;
        bool looping = false;
    };
} // namespace dgm