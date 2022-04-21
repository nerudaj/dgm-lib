#pragma once

#include <DGM/classes/ParticleSystemRenderer.hpp>
#include <DGM/classes/Buffer.hpp>
#include <DGM/classes/Window.hpp>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace dgm {
	class Time;

	namespace ps {
		class Particle;

		/**
		 *  \brief Interface and base implementation of ParticleSystem
		 *
		 *  \details Always inherit from this class when creating new
		 *  particle system.
		 */
		class ParticleSystemInterface {
		protected:
			dgm::ps::ParticleSystemRenderer renderer;
			Buffer<dgm::ps::Particle*> particles;

			/**
			 *  \brief Method for creating particle objects
			 *
			 *  This method is called automatically during init() and
			 *  serves the purpose of allocating memory for particles.
			 *  If you can work with dgm::ps::Particle, you can ignore
			 *  it, if you need ParticleSystem to work with your custom
			 *  particle type, then override this method.
			 *
			 *  Index denotes the index in particles buffer.
			 *
			 *  \warn Do not call this method by yourself! It is only
			 *  supposed to be called during init() to populate particles array.
			 */
			virtual dgm::ps::Particle* createParticle(sf::Vertex* vertices, unsigned index) const;
		
		public:
			/**
			 *  \brief Render particle system to target window
			 */
			void draw(dgm::Window &window) {
				window.draw(renderer);
			}
			
			/**
			 *  \brief Bind the texture to the particle system
			 *  
			 *  \param[in]  texture  Texture that will be used for particle rendering
			 *  
			 *  All particles must share the same texture, however they can be textured/animated
			 *  via clipping the texture.
			 */
			void setTexture(sf::Texture &texture) noexcept {
				renderer.setTexture(texture);
			}

			/**
			 *  \brief Initialize the ParticleSystem
			 *  
			 *  \param[in]  particleCount  Maximum number of particles in the system
			 *  \param[in]  factory        Factor for creating new particles
			 */
			virtual void init(const std::size_t particleCount);
			
			/**
			 *  \brief Update particle system
			 * 
			 *  \details This is the only method you have to actually
			 *  implement in your inherited particle system.
			 */
			virtual void update(const dgm::Time &time) =0;

			ParticleSystemInterface() {}
			ParticleSystemInterface(ParticleSystemInterface& other) = delete;
			ParticleSystemInterface(ParticleSystemInterface&& other) = default;
			virtual ~ParticleSystemInterface();
		};
	};
};