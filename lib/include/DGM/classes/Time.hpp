#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	/**
	 *  \brief Class representing time between frames
	 *  
	 *  It stores difference of time between last two
	 *  calls to reset.
	 */
	class Time {
	private:
		sf::Clock clock;
		sf::Time  elapsed;
		float     deltaTime;

	public:
		/**
		 *  \brief Get number of seconds between two last calls to reset
		 *  
		 *  \return Float representation of seconds, 1.f = 1s
		 */
		float getDeltaTime() const { return deltaTime; }

		/**
		 *  \brief Get time between last two calls to reset
		 */
		const sf::Time &getElapsed() const { return elapsed; }

		/**
		 *  \brief Reset the clock and store time elapsed
		 */
		void reset() {
			elapsed = clock.restart();
			deltaTime = elapsed.asSeconds();
		}

		Time() {
			reset();
		}
	};
}