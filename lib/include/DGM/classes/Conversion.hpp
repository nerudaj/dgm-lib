#pragma once

#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace dgm {
	constexpr float PIOVER180 = 0.01745329252f;
	constexpr float _180OVERPI = 57.2957795131f;

	class Conversion {
	public:
		/**
		 *  \brief Convert polar coordinates to cartesian
		 *
		 *  \param [in] angle 0° is at [0, 1], counting clockwise
		 *  \param [in] size Length of the vector
		 */
		static [[nodiscard]] sf::Vector2f polarToCartesian(float angle, float size) {
			angle *= PIOVER180;
			return sf::Vector2f(std::cos(angle), std::sin(angle)) * size;
		}

		/**
		 *  \brief Convert polar coordinates (in degrees) to cartesian
		 *
		 *  \param [in] angleSize X coord is angle, Y coord is size
		 */
		static [[nodiscard]] sf::Vector2f polarToCartesian(const sf::Vector2f &angleSize) {
			return polarToCartesian(angleSize.x, angleSize.y);
		}

		/**
		 *  \brief Convert cartesian coordinates to polar
		 *
		 *  \return X component of returned vector is angle, Y component is size
		 */
		static [[nodiscard]] sf::Vector2f cartesianToPolar(const float x, const float y);

		/**
		*  \brief Convert cartesian coordinates to polar
		*
		*  \return X component of returned vector is angle, Y component is size
		*/
		static [[nodiscard]] sf::Vector2f cartesianToPolar(const sf::Vector2f &coordinates) {
			return cartesianToPolar(coordinates.x, coordinates.y);
		}
	};
}