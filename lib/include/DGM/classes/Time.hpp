#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

namespace dgm
{
    /**
     *  \brief Class representing time between frames
     *
     *  It stores difference of time between last two
     *  calls to reset.
     */
    class [[nodiscard]] Time
    {
    protected:
        sf::Clock clock;
        sf::Time elapsed;
        float deltaTime;

    public:
        /**
         *  \brief Get number of seconds between two last calls to reset
         *
         *  \return Float representation of seconds, 1.f = 1s
         */
        NODISCARD_RESULT constexpr float getDeltaTime() const noexcept
        {
            return deltaTime;
        }

        /**
         *  \brief Get time between last two calls to reset
         */
        NODISCARD_RESULT const sf::Time& getElapsed() const noexcept
        {
            return elapsed;
        }

        /**
         *  \brief Reset the clock and store time elapsed
         */
        void reset() noexcept
        {
            elapsed = clock.restart();
            deltaTime = elapsed.asSeconds();
        }

        Time() noexcept
        {
            reset();
        }
    };
} // namespace dgm