#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cassert>

namespace dgm
{
    namespace ps
    {
        template<unsigned ParticleCount>
        class ParticleSystemRenderer final
            : public sf::Drawable
            , public sf::Transformable
        {
        public:
            [[nodiscard]] constexpr ParticleSystemRenderer()
                : sf::Drawable(), sf::Transformable()
            {
                vertices = sf::VertexArray(sf::Quads, ParticleCount * 4);
            }

        public:
            /**
             *  \brief Get array of four vertices for given particle
             *
             *  \param[in] index  Index of particle
             *  \return Pointer to array of four vertices
             */
            [[nodiscard]] constexpr sf::Vertex*
            getParticleVertices(const std::size_t index) noexcept
            {
                assert(index < vertices.getVertexCount() / 4);
                return &vertices[index * 4];
            }

            /**
             *  \brief Bind texture to the object
             *
             *  \param[in]  texture  Texture that will be used for particle
             * rendering
             *
             *  Particle system does not interface directly with bound texture,
             * it only has access to clipping data.
             */
            constexpr void setTexture(sf::Texture& newTexture) noexcept
            {
                texture = &newTexture;
            }

        private:
            virtual void
            draw(sf::RenderTarget& target, sf::RenderStates states) const
            {
                // apply the transform
                states.transform *= getTransform();

                // apply the tileset texture
                states.texture = texture;

                // draw the vertex array
                target.draw(vertices, states);
            }

        private:
            sf::VertexArray vertices;
            sf::Texture* texture = nullptr;
        };
    }; // namespace ps
};     // namespace dgm