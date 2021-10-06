#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <cassert>

namespace sf {
	class RenderTarget;
	class RenderStates;
	class Texture;
}

namespace dgm {
	namespace ps {		
		class ParticleSystemRenderer : public sf::Drawable, public sf::Transformable {
		private:
			virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
			
		protected:
			sf::VertexArray vertices;
			sf::Texture *texture = nullptr;
		
		public:
			/**
			 *  \brief Get array of four vertices for given particle
			 *  
			 *  \param[in] index  Index of particle
			 *  \return Pointer to array of four vertices
			 */
			sf::Vertex* getParticleVertices(const std::size_t index) noexcept {
				assert(index < vertices.getVertexCount() / 4);
				return &vertices[index * 4];
			}
			
			/**
			 *  \brief Initialize the object
			 *  
			 *  \param[in]  particleCount  How many particles will renderer hold
			 *  \return TRUE on success
			 */
			void init(const std::size_t particleCount) {
				vertices = sf::VertexArray(sf::Quads, particleCount * 4);
			}
			
			/**
			 *  \brief Bind texture to the object
			 *  
			 *  \param[in]  texture  Texture that will be used for particle rendering
			 *  
			 *  Particle system does not interface directly with bound texture, it only
			 *  has access to clipping data.
			 */
			void setTexture(sf::Texture& newTexture) noexcept {
				texture = &newTexture;
			}
		};
	};
};