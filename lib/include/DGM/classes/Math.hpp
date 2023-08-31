#pragma once

#include <DGM\dgm.hpp>
#include <cassert>
#include <cmath>
#include <numbers>
#include <optional>

namespace dgm
{
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

        enum class AngleType
        {
            Degrees,
            Radians
        };

    public:
        struct PolarCoords
        {
            float angle = 0.f;
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
         *  \brief Compute size of the vector
         *
         *  \return Size of the vector
         */
        [[nodiscard]] static inline float
        getSize(const sf::Vector2f& vec) noexcept
        {
            return std::sqrt(vec.x * vec.x + vec.y * vec.y);
        }

        /**
         *  \brief Get unit vector that has the same direction
         *  as provided vector
         *
         *  \return Either valid unit vector or [0, 0] if the source
         *  vector was also [0, 0]
         */
        [[nodiscard]] static sf::Vector2f
        toUnit(const sf::Vector2f& vec) noexcept
        {
            const float size = getSize(vec);
            if (size == 0.f) return vec;
            return vec / size;
        }

        /**
         * \brief Get vector rotated by given angle
         * 
         * By default, the angle is in degrees, but you
         * can change that to radians by specifying template
         * parameter:
         * 
         \code
         auto&& rotated = getRotated<dgm::Math::AngleType::Radians>(vec, angle);
         \endcode
         */
        template<AngleType AT = AngleType::Degrees>
        [[nodiscard]] static inline sf::Vector2f
        getRotated(const sf::Vector2f& vec, float angle) noexcept
        {
            if constexpr (AT == AngleType::Degrees)
            {
                angle *= piover180_v<float>;
            }
            const float cosVal = std::cos(angle);
            const float sinVal = std::sin(angle);
            return sf::Vector2f(
                vec.x * cosVal - vec.y * sinVal,
                vec.x * sinVal + vec.y * cosVal);
        }

        /**
         *  \brief Computes dot product of two vectors
         * 
         *  Dot product is a sum of memberwise multiplication
         *  a.x * b.x + a.y * b.y
         *
         *  \return Dot product of input vectors
         */
        [[nodiscard]] static constexpr float getDotProduct(
            const sf::Vector2f& vectorA,
            const sf::Vector2f& vectorB) noexcept
        {
            return vectorA.x * vectorB.x + vectorA.y * vectorB.y;
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
         * \return TRUE if values are equal within a given precision
         */
        [[nodiscard]] static inline bool isApproxEqual(
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
        [[nodiscard]] static constexpr float
        getRadialDifference(const float alfa, const float beta) noexcept
        {
            return std::min(
                std::abs(alfa - beta),
                std::min(alfa, beta) + 360.f - std::max(alfa, beta));
        }

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

        /**
         *  \brief Convert polar coordinates to cartesian
         *
         *  \param [in] angle 0° is at [0, 1], counting clockwise
         *  \param [in] size Length of the vector
         * 
         *  Angle is by default in degrees, but this can be changed
         *  to radians by template parameter:
         *
         \code
         auto&& rotated = polarToCartesian<dgm::Math::AngleType::Radians>(vec, angle);
         \endcode
         */
        template<AngleType AT = AngleType::Degrees>
        [[nodiscard]] static sf::Vector2f
        polarToCartesian(float angle, float length) noexcept
        {
            if constexpr (AT == AngleType::Degrees)
            {
                angle *= piover180_v<float>;
            }
            return sf::Vector2f(std::cos(angle), std::sin(angle)) * length;
        }

        /**
         *  \brief Convert polar coordinates (in degrees) to cartesian
         * 
         *  Angle is by default in degrees, but this can be changed
         *  to radians by template parameter:
         *
         \code
         auto&& rotated = polarToCartesian<dgm::Math::AngleType::Radians>(vec, angle);
         \endcode
         */
        template<AngleType AT = AngleType::Degrees>
        [[nodiscard]] static sf::Vector2f
        polarToCartesian(const PolarCoords& coords) noexcept
        {
            return polarToCartesian<AT>(coords.angle, coords.length);
        }

        /**
         *  \brief Convert cartesian coordinates to polar
         * 
         *  Angle is by default in degrees, but this can be changed
         *  to radians by template parameter:
         *
         \code
         auto&& rotated = cartesianToPolar<dgm::Math::AngleType::Radians>(vec, angle);
         \endcode
         */
        template<AngleType AT = AngleType::Degrees>
        [[nodiscard]] static inline PolarCoords
        cartesianToPolar(const float x, const float y) noexcept
        {
            return cartesianToPolar<AT>({ x, y });
        }

        /**
         *  \brief Convert cartesian coordinates to polar
         * 
         *  Angle is by default in degrees, but this can be changed
         *  to radians by template parameter:
         *
         \code
         auto&& rotated = cartesianToPolar<dgm::Math::AngleType::Radians>(vec, angle);
         \endcode
         */
        template<AngleType AT = AngleType::Degrees>
        [[nodiscard]] static PolarCoords
        cartesianToPolar(const sf::Vector2f& coords) noexcept
        {
            const float size = getSize(coords);
            float angle = getAngle(coords.x, coords.y);
            if constexpr (AT == AngleType::Degrees)
            {
                angle *= _180overpi_v<float>;
            }
            return { angle, size };
        }

    private:
        static float getAngle(const float x, const float y);
    };
}; // namespace dgm