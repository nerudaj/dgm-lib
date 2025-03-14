#pragma once

#include <DGM/classes/Collision.hpp>
#include <DGM/classes/Compatibility.hpp>
#include <DGM/classes/Objects.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <functional>

namespace dgm
{
    class Time;

    /**
     * \brief Class for manipulating sf::View transformation
     *
     *  This class can be used both for player driven transformation
     *  as well as animated transitions and effects (screenshake).
     *  All animations are adjustable via easing functions. If you
     *  don't know what that is, you can always start with identity
     *  function.
     */
    class [[nodiscard]] Camera final
    {
    public:
        /// Easing function. It is supposed to accept a number from 0..1 and
        /// return a number from 0..1
        using EasingFunc = std::function<float(float)>;

    public:
        Camera(const sf::FloatRect& viewport, const sf::Vector2f& resolution)
            : view(view)
        {
            defaultView.setViewport(viewport);
            defaultView.setSize(resolution);
            defaultView.setCenter(resolution / 2.f);
            view = defaultView;
        }

        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;

    protected:
        /**
         *  Container holding transform effect
         */
        template<typename T>
        struct Effect
        {
            float elapsed = 0.f;  // How much time elapsed in seconds
            float duration = 0.f; // How much time effect should take in seconds
            EasingFunc easing;    // Easing functions for animation
            T start = T();        // Starting value for animation
            T amount = T();       // Offset value from start to end

            /**
             *  Update elapsed and use it to compute
             *  parameter t for animation (also ease
             *  this parameter using EasingFunc).
             */
            float update(const dgm::Time& time);

            /**
             * Is effect still animating?
             */
            [[nodiscard]] constexpr bool isActive() const noexcept
            {
                return elapsed < duration;
            }

            /**
             *  Shorthand for initialization of this object
             */
            void
            init(T start, T amount, const sf::Time& duration, EasingFunc f);
        };

        /**
         *  Dedicated structure for screen shake effect. It should rapidly move
         * the camera around a center point in random directions. For this
         * purpose we use predefined set of randomly generated numbers.
         */
        struct ShakeEffect : public Effect<sf::Vector2f>
        {
            float hold =
                0.f; // how long should position be held before changing
        };

        Effect<sf::Vector2f> moveEffect;
        Effect<float> zoomEffect;
        Effect<sf::Angle> rotationEffect;
        ShakeEffect shakeEffect;

        NODISCARD_RESULT dgm::Rect getViewBoundingBox() const
        {
            return dgm::Rect(
                view.getCenter() - view.getSize() / 2.f, view.getSize());
        }

    public:
        NODISCARD_RESULT constexpr const sf::View&
        getCurrentView() const noexcept
        {
            return view;
        }

        NODISCARD_RESULT constexpr const sf::View&
        getDefaultView() const noexcept
        {
            return view;
        }

        /**
         *  \brief True if camera is animating movement
         */
        NODISCARD_RESULT constexpr bool isMoving() const noexcept
        {
            return moveEffect.isActive();
        }

        /**
         *  \brief True if camera is animating zooming
         */
        NODISCARD_RESULT constexpr bool isZooming() const noexcept
        {
            return zoomEffect.isActive();
        }

        /**
         *  \brief True if camera is animating rotation
         */
        NODISCARD_RESULT constexpr bool isRotating() const noexcept
        {
            return rotationEffect.isActive();
        }

        /**
         *  \brief True if camera is animating shaking
         */
        NODISCARD_RESULT constexpr bool isShaking() const noexcept
        {
            return shakeEffect.isActive();
        }

        NODISCARD_RESULT bool
        isObjectVisible(const dgm::Rect& object) const noexcept
        {
            return dgm::Collision::basic(getViewBoundingBox(), object);
        }

        NODISCARD_RESULT bool
        isObjectVisible(const dgm::Circle& object) const noexcept
        {
            return dgm::Collision::basic(getViewBoundingBox(), object);
        }

        /**
         *  \brief Update all currently animated camera effects
         */
        void update(const dgm::Time& time);

        /**
         *  \brief Changes position of camera
         *
         *  Will mess up if camera isMoving
         */
        inline void setPosition(const sf::Vector2f& position)
        {
            view.setCenter(position);
        }

        /**
         *  \brief Changes position of camera, relative to where it is now
         *
         *  Will mess up if camera isMoving
         */
        inline void move(const sf::Vector2f& position)
        {
            view.move(position);
        }

        void resetTransformations()
        {
            view = defaultView;
        }

        /**
         *  \brief Changes zoom level of camera, relative to initial zoom level
         *
         *  SFML zoom works by scaling the currently set scale (so repeated call
         *  to zoom(0.25f) will always change the zoom level). This function is
         *  indempotent, subsequent calls with same value won't change the scale
         *  anymore.
         *
         *  Will mess up if camera isZooming
         */
        inline void setZoom(float level)
        {
            view.setSize(defaultView.getSize());
            view.zoom(level);
        }

        /**
         *  \brief Sets the rotation of the camera
         *
         *  Will mess up if camera isRotating
         */
        inline void setRotation(sf::Angle angle)
        {
            view.setRotation(angle);
        }

        /**
         *  \brief Perform animated transition to another position
         *
         *  This transition animates for 'duration' of time, using easing
         *  function 'f' (so you define smoothstop/start animation).
         *
         *  This function does nothing if camera isMoving
         */
        void moveGradually(
            const sf::Vector2f& position,
            const sf::Time& duration,
            EasingFunc f);

        /**
         *  \brief Perform animated zoom to another zoom level
         *
         *  This zoom animates for 'duration' of time, using easing
         *  function 'f' (so you define smoothstop/start animation).
         *
         *  This function does nothing if camera isZooming
         */
        void zoomGradually(float level, const sf::Time& duration, EasingFunc f);

        /**
         *  \brief Perform animated rotation by given angle
         *
         *  This rotation animates for 'duration' of time, using easing
         *  function 'f' (so you define smoothstop/start animation).
         *
         *  This function does nothing if camera isRotating
         */
        void rotateGradually(
            sf::Angle angle, const sf::Time& duration, EasingFunc f);

        /**
         *  \brief Perform screenshake
         *
         *  The screenshake happens for 'duration' of time. Power of the
         *  shake is defined by amount (how many pixels camera can deviate
         *  from center). You can use easing function to gradually
         * decrease/increase shake intensity over time. 'hold' defines how
         * rapidly camera changes positions during shaking. Default value is
         * generally ok.
         *
         *  This function does nothing if camera isShaking
         */
        void shake(
            const sf::Time& duration,
            float amount,
            EasingFunc f = [](float) -> float { return 1.f; },
            const sf::Time& hold = sf::milliseconds(20));

    protected:
        sf::View view;
        sf::View defaultView;
    };
} // namespace dgm