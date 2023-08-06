#pragma once

#include <DGM\dgm.hpp>
#include <cassert>
#include <cmath>
#include <numbers>
#include <optional>

namespace dgm
{
    /**
     *  \brief Class with auxiliary mathematical methods
     */
    class Math
    {
    public:
        /**
         *  \brief Compute size of the vector
         *
         *  \param[in]  vect  Vector object
         *
         *  \return Size of vect
         */
        [[nodiscard]] static float vectorSize(const sf::Vector2f& vect) noexcept
        {
            return sqrt(vect.x * vect.x + vect.y * vect.y);
        }

        [[nodiscard]] static float
        vectorSize(const float x, const float y) noexcept
        {
            return vectorSize({ x, y });
        }

        [[nodiscard]] static sf::Vector2f
        rotateVector(const sf::Vector2f& vec, float angle) noexcept
        {
            const float rad = angle * std::numbers::pi_v<float> / 180.f;
            return sf::Vector2f(
                vec.x * std::cos(rad) - vec.y * std::sin(rad),
                vec.x * std::sin(rad) + vec.y * std::cos(rad));
        }

        /**
         *  \brief Computes dot product of two vectors
         *
         *  \param[in]  vectorA  Vector object
         *  \param[in]  vectorB  Vector object
         *
         *  \return Dot product of vectorA and vectorB
         */
        [[nodiscard]] static constexpr float dotProduct(
            const sf::Vector2f& vectorA, const sf::Vector2f& vectorB) noexcept
        {
            return vectorA.x * vectorB.y + vectorA.x * vectorB.y;
        }

        /**
         *  \brief Map the value from interval X to interval Y (linearly)
         *
         *  \param[in]  value   Value to map
         *  \param[in]  startX  Start of X interval
         *  \param[in]  endX    End of X interval
         *  \param[in]  startY  Start of Y interval
         *  \param[in]  endY    End of Y interval
         */
        [[nodiscard]] static constexpr float
        map(const float value,
            const float startX,
            const float endX,
            const float startY,
            const float endY) noexcept
        {
            return (endY - startY) / (endX - startX) * (value - startX)
                   + startY;
        }

        /**
         * \brief Test whether a float value is approximately equal to another
         * float value
         *
         * \param[in]  value          Value to test
         * \param[in]  expectedValue  Expected value
         * \param[in]  precision      Precision of comparison
         *
         * \return TRUE if values are equal within a given precision
         */
        static bool approxEqual(
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
        [[nodiscard]] static float
        radialDifference(const float alfa, const float beta) noexcept
        {
            return std::min(
                std::abs(alfa - beta),
                std::min(alfa, beta) + 360 - std::max(alfa, beta));
        }

        /**
         *  Structure representing a mathematical line, infinitely
         *  stretching to both directions. A line is defined by a vector
         *  that has the same direction as the line and a point that lies
         *  on the line.
         *
         *  Such line can be represented by an equation ax + by + c = 0
         *  where [a, b] is a vector perpendicular to the line direction
         *  and c = a * point.x + b * point.y for a point that lies on
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

        /**
         *  First a point that lies on the line and has the least
         *  distance to the given point.
         */
        [[nodiscard]] static sf::Vector2f getClosestPointOnLine(
            const Line& line, const sf::Vector2f& point) noexcept;

        /**
         * If two lines intersect, returns a point where they are
         * intersecting (or nullopt if they don't).
         */
        [[nodiscard]] static std::optional<sf::Vector2f>
        getIntersection(const Line& line1, const Line& line2) noexcept;

        /**
         *  If line intersects a circle, this function returns a pair
         *  of point where the circle is intersected. Otherwise returns
         *  a nullopt.
         */
        [[nodiscard]] static std::optional<
            std::pair<sf::Vector2f, sf::Vector2f>>
        getIntersection(const Line& line, const dgm::Circle& circle);

        /**
         *  Returns true if line intersects a circle. This call is
         *  faster than calling getIntersection and checking if the
         *  optional contains a result.
         */
        [[nodiscard]] static bool
        hasIntersection(const Line& line, const dgm::Circle& circle);
    };
}; // namespace dgm