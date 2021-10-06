#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	namespace ps {
		/**
		 *  \brief Particle abstraction
		 *  
		 *  \details This class represents a single particle which
		 *  we can move around, scale or even animate it. 
		 * 
		 *  You can also track its lifetime.
		 * 
		 *  Inherit from this class if you want to create your custom Particles.
		 *  To texture your particles, see ParticleSystem::setTexture and
		 *  Particle::setAnimationFrame.
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

			/**
			 *  \brief Get forward vector of the particle
			 */
			[[nodiscard]] const sf::Vector2f &getForward() const noexcept {
				return forward;
			}

			/**
			 *  \brief Is the particle alive
			 *  
			 *  \details Test whether lifespan is greater than zero
			 */
			[[nodiscard]] constexpr bool isAlive() const noexcept {
				return lifespan > 0.f;
			}

			/**
			 *  \brief Change animation frame displayed on particle
			 *  
			 *  \details Change textCoords of each quads vertex
			 */
			virtual void setAnimationFrame(const sf::IntRect &frame) noexcept;

			/**
			 *  \brief Set forward vector of the particle 
			 */
			virtual void setForward(const sf::Vector2f& f) noexcept {
				forward = f;
			}

			/**
			 *  \brief Sets the color of the particle
			 */
			virtual void setColor(const sf::Color& color) noexcept {
				for (unsigned i = 0; i < 4; i++) quad[i].color = color;
			}

			/**
			 *  \brief Move the particle in direction of forward
			 * 
			 *  Typically, you want to first retrieve forward of this particle
			 *  add some global force to it (like gravity), then multiply it
			 *  with deltaTime since last frame and pass the resulting value
			 *  to this function.
			 */
			virtual void moveForwardBy(const sf::Vector2f& fwd) noexcept {
				for (unsigned i = 0; i < 4; i++) quad[i].position += fwd;
			}

			/**
			 *  \brief Decrement lifespan by given time difference
			 */
			virtual void updateLifespan(const sf::Time& deltaTime) noexcept {
				lifespan -= deltaTime.asSeconds();
			}

			/**
			 *  \brief Spawns the particle at given position
			 *  
			 *  Size of the particle must be set prior to calling
			 *  this method.
			 */
			virtual void spawn(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Time &lifespan);

			/**
			 *  \brief Initialize the object with pointer to its vertices
			 *
			 *  \param [in] vertices Pointer to topleft vertex of particle
			 */
			Particle(sf::Vertex* vertices) : quad(vertices) {}

			virtual ~Particle() {}
		};
	};
}
