#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	namespace ps {
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

			float randomFloat(float min, float max);

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
			virtual dgm::ps::Particle* createParticle(unsigned index) const;
		
		public:
			/**
			 *  \brief Render particle system to target window
			 */
			void draw(dgm::Window &window) { window.draw(renderer); }
		
			/**
			 *  \brief Get reference to renderer object
			 */
			dgm::ps::ParticleSystemRenderer &getRenderer() { return renderer; }
			
			/**
			*  \brief Get reference to renderer object
			*/
			const dgm::ps::ParticleSystemRenderer &getRenderer() const { return renderer; }
			
			/**
			 *  \brief Bind the texture to the particle syste
			 *  
			 *  \param[in]  texture  Texture that will be used for particle rendering
			 *  
			 *  If you only need white rectangles, you don't have to bind texture to
			 *  this object.
			 */
			void setTexture(sf::Texture &texture) { renderer.setTexture(texture); }

			/**
			 *  \brief Initialize the ParticleSystem
			 *  
			 *  \param[in]  particleCount  Maximum number of particles in the system
			 *  \param[in]  factory        Factor for creating new particles
			 *  \return TRUE on success
			 */
			virtual bool init(const std::size_t particleCount);
			
			/**
			 *  \brief Update particle system
			 * 
			 *  \details This is the only method you have to actually
			 *  implement in your inherited particle system.
			 */
			virtual void update(const dgm::Time &time) =0;

			ParticleSystemInterface();
			~ParticleSystemInterface();
		};
	};
};