#pragma once

#include <DGM\dgm.hpp>
#include <cassert>
#include <cmath>

namespace dgm
{
    /**
     *  \brief Class with auxiliary mathematical methods
     */
    class Math
    {
    public:
        constexpr static float PI = 3.1415926536f;
        constexpr static float PIOVER180 =
            PI / 180.f; /// multiply this with degrees to get radians
        constexpr static float _180OVERPI =
            180.f / PI; /// multiply this with radians to get degrees

        /**
         *  \brief Compute size of the vector
         *
         *  \param[in]  vect  Vector object
         *
         *  \return Size of vect
         */
        static float vectorSize(const sf::Vector2f& vect)
        {
            return sqrt(vect.x * vect.x + vect.y * vect.y);
        }

        static float vectorSize(const float x, const float y)
        {
            return vectorSize({ x, y });
        }

        static sf::Vector2f rotateVector(const sf::Vector2f& vec, float angle)
        {
            const float rad = angle * PIOVER180;
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
        static float
        dotProduct(const sf::Vector2f& vectorA, const sf::Vector2f& vectorB)
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
        static float
        map(const float value,
            const float startX,
            const float endX,
            const float startY,
            const float endY)
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
            const float precision = 0.01f)
        {
            return std::abs(expectedValue - value) <= precision;
        }

        /**
         * \brief Compute difference between two angles, wrapping over 360°
         *
         * \param[in] alfa  Angle in degrees in interval <0, 360>
         * \param[in] beta  Angle in degrees in interval <0, 360>
         */
        static float radialDifference(const float alfa, const float beta)
        {
            return std::min(
                std::abs(alfa - beta),
                std::min(alfa, beta) + 360 - std::max(alfa, beta));
        }
    };
}; // namespace dgm