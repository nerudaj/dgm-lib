#pragma once

#include <DGM/classes/Error.hpp>
#include <DGM/classes/Particle.hpp>
#include <DGM/classes/ParticleSystemRenderer.hpp>
#include <DGM/classes/StaticBuffer.hpp>
#include <DGM/classes/Time.hpp>
#include <DGM/classes/Window.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <concepts>
#include <span>

namespace dgm
{
    namespace ps
    {
        /**
         *  \brief Interface and base implementation of ParticleSystem
         *
         *  \details Always inherit from this class when creating new
         *  particle system.
         */
        class [[nodiscard]] ParticleSystemInterface
        {
        public:
            explicit ParticleSystemInterface(unsigned particleCount)
                : renderer(particleCount), particles(particleCount)
            {
                try
                {
                    for (unsigned i = 0; i < particles.getCapacity(); i++)
                    {
                        particles[i] =
                            createParticle(renderer.getParticleVertices(i), i);
                    }
                }
                catch (std::exception& e)
                {
                    throw dgm::Exception(e.what());
                }
            }

            ParticleSystemInterface(ParticleSystemInterface& other) = delete;
            ParticleSystemInterface(ParticleSystemInterface&& other) = default;
            virtual ~ParticleSystemInterface() = default;

        public:
            /**
             *  \brief Render particle system to target window
             */
            inline void draw(dgm::Window& window)
            {
                window.draw(renderer);
            }

            /**
             *  \brief Bind the texture to the particle system
             *
             *  \param[in]  texture  Texture that will be used for particle
             * rendering
             *
             *  All particles must share the same texture, however they can be
             * textured/animated via clipping the texture.
             */
            constexpr void setTexture(const sf::Texture& texture) noexcept
            {
                renderer.setTexture(texture);
            }

            /**
             *  \brief Update particle system
             *
             *  \details This is the only method you have to actually
             *  implement in your inherited particle system.
             */
            virtual void update(const dgm::Time& time) = 0;

        protected:
            NODISCARD_RESULT inline std::unique_ptr<dgm::ps::Particle>
            createParticle(std::span<sf::Vertex> vertices, unsigned index) const
            {
                return createParticleImpl(vertices, index);
            }

            /**
             *  \brief Method for creating particle objects
             *
             *  \param [in] vertices  Vertices for particle from VBO
             *  \param [in] id        Unique ID of particle
             *
             *  This method is called automatically in constructor and
             *  serves the purpose of allocating memory for particles.
             *  If you can work with dgm::ps::Particle, you can ignore
             *  it, if you need ParticleSystem to work with your custom
             *  particle type, then override this method.
             *
             *  Index denotes the index in particles buffer.
             *
             *  \warn Do not call this method by yourself! It is only
             *  supposed to be called during construction to populate particles
             * array.
             */
            [[nodiscard]] virtual std::unique_ptr<dgm::ps::Particle>
            createParticleImpl(std::span<sf::Vertex> vertices, unsigned) const
            {
                return std::make_unique<Particle>(vertices);
            }

        protected:
            dgm::ps::ParticleSystemRenderer renderer;
            dgm::StaticBuffer<std::unique_ptr<Particle>> particles;
        };
    }; // namespace ps
}; // namespace dgm