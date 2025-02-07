#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cassert>
#include <span>

namespace dgm
{
    namespace ps
    {
        class ParticleSystemRenderer final
            : public sf::Drawable
            , public sf::Transformable
        {
        public:
            [[nodiscard]] explicit ParticleSystemRenderer(
                unsigned particleCount)
                : sf::Drawable(), sf::Transformable()
            {
                vertices = sf::VertexArray(
                    sf::PrimitiveType::Triangles,
                    particleCount * VERTICES_PER_QUAD);
            }

        public:
            /**
             *  \brief Get array of vertices for given particle
             *
             *  \param[in] index  Index of particle
             */
            [[nodiscard]] std::span<sf::Vertex>
            getParticleVertices(const std::size_t index) noexcept
            {
                assert(index < vertices.getVertexCount() / VERTICES_PER_QUAD);
                return std::span(
                    &vertices[index * VERTICES_PER_QUAD], VERTICES_PER_QUAD);
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
            constexpr void setTexture(const sf::Texture& newTexture) noexcept
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
            constexpr static inline const unsigned VERTICES_PER_QUAD = 6;

            sf::VertexArray vertices;
            const sf::Texture* texture = nullptr;
        };
    }; // namespace ps
}; // namespace dgm