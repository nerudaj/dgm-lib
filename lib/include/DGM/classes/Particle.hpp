#pragma once

#include <DGM/classes/Compatibility.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>

namespace dgm
{
    namespace ps
    {
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
        class [[nodiscard]] Particle
        {
        public:
            /**
             *  \brief Initialize the object with pointer to its vertices
             *
             *  \param [in] vertices Pointer to topleft vertex of particle
             */
            explicit Particle(std::span<sf::Vertex> vertices) noexcept
                : vertices(vertices)
            {
            }

            virtual ~Particle() = default;

        public:
            /**
             *  \brief Get position of particle (center of the particle)
             */
            [[nodiscard]] sf::Vector2f getPosition() const noexcept
            {
                return (vertices[0].position + vertices[4].position) / 2.f;
            }

            /**
             *  \brief Get forward vector of the particle
             */
            [[nodiscard]] const sf::Vector2f& getForward() const noexcept
            {
                return forward;
            }

            [[nodiscard]] constexpr sf::Angle getRotation() const noexcept
            {
                return rotation;
            }

            /**
             *  \brief Is the particle alive
             *
             *  \details Test whether lifespan is greater than zero
             */
            [[nodiscard]] constexpr bool isAlive() const noexcept
            {
                return lifespan > 0.f;
            }

            /**
             *  \brief Change animation frame displayed on particle
             *
             *  \details Change textCoords of each quads vertex
             */
            virtual void setAnimationFrame(const sf::IntRect& frame) noexcept;

            /**
             *  \brief Set forward vector of the particle
             */
            virtual void setForward(const sf::Vector2f& f) noexcept
            {
                forward = f;
            }

            virtual void setRotation(const sf::Angle angle) noexcept;

            /**
             *  \brief Sets the color of the particle
             */
            virtual void setColor(const sf::Color& color) noexcept
            {
                for (auto&& vertex : vertices)
                    vertex.color = color;
            }

            /**
             *  \brief Move the particle in direction of forward
             *
             *  Typically, you want to first retrieve forward of this particle
             *  add some global force to it (like gravity), then multiply it
             *  with deltaTime since last frame and pass the resulting value
             *  to this function.
             */
            virtual void moveForwardBy(const sf::Vector2f& fwd) noexcept
            {
                for (auto&& vertex : vertices)
                    vertex.position += fwd;
            }

            /**
             *  \brief Rotate particle by a given angle
             */
            virtual void rotateBy(const sf::Angle angle) noexcept
            {
                setRotation(rotation + angle);
            }

            /**
             *  \brief Decrement lifespan by given time difference
             */
            virtual void updateLifespan(const sf::Time& deltaTime) noexcept
            {
                lifespan -= deltaTime.asSeconds();
            }

            /**
             *  \brief Add value to forward
             *
             *  Just shorthand for setForward(getForward() + fwd)
             */
            virtual void addToForward(const sf::Vector2f& fwd)
            {
                forward += fwd;
            }

            /**
             *  \brief Spawns the particle at given position
             *
             *  Size of the particle must be set prior to calling
             *  this method.
             */
            virtual void spawn(
                const sf::Vector2f& position,
                const sf::Vector2f& size,
                const sf::Time& lifespan = sf::Time::Zero);

            /**
             *  \brief Despawn the particle so it's no longer rendered
             */
            virtual void despawn()
            {
                spawn({ 0.f, 0.f }, { 0.f, 0.f }, sf::Time::Zero);
            }

        protected:
            std::span<sf::Vertex> vertices;
            float lifespan = 1.f;                 ///< How long till dead
            sf::Angle rotation = sf::Angle::Zero; ///< Current angle
            float diagonalHalfLength =
                1.f; ///< Each particle is a square with this being the half of
                     ///< diagonal length, used for rotation calculations
            sf::Vector2f forward = { 0.f,
                                     0.f }; ///< Direction of particle movement
        };
    }; // namespace ps
} // namespace dgm
