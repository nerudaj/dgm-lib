#pragma once

#include <DGM/classes/Clip.hpp>
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
    class Animation
    {
    private:
        std::reference_wrapper<const AnimationStates> states;
        sf::Time elapsedTime = sf::seconds(0);
        sf::Time timePerFrame = sf::seconds(0);
        std::size_t currentFrameIndex = 0;
        AnimationStates::const_iterator currentState;
        bool looping = false;

        [[nodiscard]] bool isCurrentStateValid() const noexcept
        {
            return currentState != states.get().end();
        }

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
        bool update(const dgm::Time& time);

        /**
         *  \brief Set the new animation state
         *
         *  \details Animation will play from the first
         *  frame of the new animation state. If looping is set
         *  to true then animation will start over when it finishes
         *  and \ref update will always return true.
         */
        void setState(const std::string& state, bool looping = false);

        /**
         *  \brief Set speed of animation in frames per second
         */
        void setSpeed(unsigned framesPerSecond);

        constexpr void setLooping(bool enabled) noexcept
        {
            looping = enabled;
        }

        /**
         *  \brief Get speed as number of frames per second
         */
        [[nodiscard]] unsigned getSpeed() const
        {
            return static_cast<unsigned>(
                1000.f / timePerFrame.asMilliseconds());
        }

        /**
         *  \brief Get name of currently selected state
         */
        [[nodiscard]] const std::string& getStateName() const noexcept
        {
            return currentState->first;
        }

        [[nodiscard]] constexpr bool isLooping() const noexcept
        {
            return looping;
        }

        [[nodiscard]] const sf::IntRect& getCurrentFrame() const noexcept
        {
            return currentState->second.getFrame(currentFrameIndex);
        }

        /**
         *  \brief Reset the current state of animation to frame 0
         */
        void reset() noexcept
        {
            currentFrameIndex = 0;
            elapsedTime = sf::Time::Zero;
        }

        /**
         *  \brief Construct empty animation
         *
         *  \warning Default constructor is meant only for cases when you cannot
         * construct animation immediately and have to postpone it to later
         * time, but you don't need to use the object in the meantime.
         */
        Animation();

        /**
         *  \brief Construct animation object from states and required FPS
         */
        Animation(const AnimationStates& states, int framesPerSecond = 30);
    };
} // namespace dgm