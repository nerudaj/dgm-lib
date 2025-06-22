#pragma once

#include <DGM/classes/Compatibility.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cassert>
#include <cmath>
#include <numbers>
#include <optional>

namespace dgm
{
    class Circle;

    /**
     *  \brief Various mathematical functions, mainly
     *  for working with vectors
     */
    class Math
    {
    public:
        template<typename T>
        constinit static inline T piover180_v =
            std::numbers::pi_v<T> / T { 180 };

        template<typename T>
        constinit static inline T _180overpi_v =
            std::numbers::inv_pi_v<T> * T { 180 };

    public:
        struct [[nodiscard]] PolarCoords final
        {
            sf::Angle angle = sf::Angle::Zero;
            float length = 0.f;
        };

        /**
         *  Structure representing a mathematical line, infinitely
         *  stretching to both directions. A line is defined by a vector
         *  that has the same direction as the line and a point that lies
         *  on the line.
         *
         *  Such line can be represented by an equation ax + by + c = 0
         *  where [a, b] is a vector perpendicular to the line direction
         *  and c = -(a * point.x + b * point.y) for a point that lies on
         *  a line.
         */
        struct Line
        {
            float a, b, c;

            [[nodiscard]] constexpr Line(
                const sf::Vector2f& direction,
                const sf::Vector2f& point) noexcept
            {
                a = direction.y;
                b = -direction.x;
                // At least one coord must be non-zero
                assert(!(a == 0.f && b == 0.f));
                c = -(a * point.x + b * point.y);
            }
        };

    public:
        /**
         *  \brief Get unit vector that has the same direction
         *  as provided vector
         *
         *  \return Either valid unit vector or [0, 0] if the source
         *  vector was also [0, 0]
         */
        NODISCARD_RESULT static sf::Vector2f
        toUnit(const sf::Vector2f& vec) noexcept
        {
            const float size = vec.length();
            if (size == 0.f) return vec;
            return vec / size;
        }

        /**
         * \brief Test whether a float value is approximately equal to another
         * float value
         *
         * \return TRUE if values are equal within a given precision
         */
        NODISCARD_RESULT static inline bool isApproxEqual(
            const float value,
            const float expectedValue,
            const float precision = 0.01f) noexcept
        {
            return std::abs(expectedValue - value) <= precision;
        }

        /**
         * \brief Compute difference between two angles, wrapping over 360°
         *
         * \param[in] alfa  Angle in degrees in interval <0, 360>
         * \param[in] beta  Angle in degrees in interval <0, 360>
         */
        NODISCARD_RESULT static constexpr sf::Angle
        getRadialDifference(const sf::Angle alfa, const sf::Angle beta) noexcept
        {
            const sf::Angle diff = (alfa - beta).wrapUnsigned();
            return diff > sf::degrees(180) ? sf::degrees(360) - diff : diff;
        }

        /**
         *  First a point that lies on the line and has the least
         *  distance to the given point.
         */
        NODISCARD_RESULT static sf::Vector2f getClosestPointOnLine(
            const Line& line, const sf::Vector2f& point) noexcept;

        /**
         * If two lines intersect, returns a point where they are
         * intersecting (or nullopt if they don't).
         */
        NODISCARD_RESULT static std::optional<sf::Vector2f>
        getIntersection(const Line& line1, const Line& line2) noexcept;

        /**
         *  If line intersects a circle, this function returns a pair
         *  of point where the circle is intersected. Otherwise returns
         *  a nullopt.
         */
        NODISCARD_RESULT static std::optional<
            std::pair<sf::Vector2f, sf::Vector2f>>
        getIntersection(const Line& line, const dgm::Circle& circle);

        /**
         *  Returns true if line intersects a circle. This call is
         *  faster than calling getIntersection and checking if the
         *  optional contains a result.
         */
        NODISCARD_RESULT static bool
        hasIntersection(const Line& line, const dgm::Circle& circle);

        /**
         *  \brief Convert cartesian coordinates to polar
         */
        NODISCARD_RESULT static inline PolarCoords
        cartesianToPolar(const sf::Vector2f& coords) noexcept
        {
            return PolarCoords {
                getAngle(coords.x, coords.y),
                coords.length(),
            };
        }

    private:
        static sf::Angle getAngle(const float x, const float y);
    };
}; // namespace dgm