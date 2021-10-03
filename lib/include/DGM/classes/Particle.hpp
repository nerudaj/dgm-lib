#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	namespace ps {
		/**
		 *  \brief Particle abstraction
		 *  
		 *  \details This class represents a single particle which
		 *  we can move around, scale or even animate it. You can
		 *  also have a particle with a limited (or conditioned)
		 *  lifespan.
		 */
		class Particle {
		protected:
			sf::Vertex *quad; ///< Pointer to first of the four quad vertices
			float lifespan = 1.f; ///< How long till dead
			sf::Vector2f forward = { 0.f, 0.f }; ///< Direction of particle movement
			sf::Vector2f size = { 1.f, 1.f }; ///< Particle render size. Must be set before calling spawn()

		public:
			/**
			 *  \brief Get position of particle (center of the particle)
			 */
			[[nodiscard]] sf::Vector2f getPosition() const noexcept {
				return quad[0].position + size / 2.f;
			}

			void setPosition(const sf::Vector2f& pos) noexcept;

			/**
			 *  \brief Is the particle alive
			 *  
			 *  \details Test whether lifespan is greater than zero
			 */
			bool isAlive() const { return (lifespan > 0.f); }

			/**
			 *  \brief Change animation frame displayed on particle
			 *  
			 *  \details Change textCoords of each quads vertex
			 */
			void changeFrame(const sf::IntRect &frame);

			/**
			 *  \brief Sets the color of the particle
			 */
			void setColor(const sf::Color &color);

			/**
			 *  \brief Spawns the particle at given position
			 *  
			 *  Size of the particle must be set prior to calling
			 *  this method.
			 */
			virtual void spawn(const sf::Vector2f &position);

			/**
			 *  \brief Move the particle in direction of forward
			 */
			virtual void move(const sf::Vector2f &forward);

			/**
			 *  \brief Destroys the particle
			 *
			 *  \details Until next call to spawn(), particle will not be visible
			 *  and will not be updated in any way. Frame still can be changed.
			 */
			virtual void destroy();

			/**
			 *  \brief Initialize the object with pointer to its vertices
			 *
			 *  \param [in] vertices Pointer to topleft vertex of particle
			 *
			 *  \details This method must be called prior to any other. If
			 *  you use the ParticleSystem template, particles are properly
			 *  initialized already.
			 */
			virtual void init(sf::Vertex *vertices);

			virtual ~Particle() {}
		};
	};
}
