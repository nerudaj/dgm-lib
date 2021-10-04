#pragma once

#include <DGM\dgm.hpp>
#include <optional>

namespace dgm {
	class Conversion {
	public:
		/**
		 * \brief Takes a string with hex representation of a color (#XXXXXX or #XXX)
		 * and converts it to output sf::Color
		 *
		 * If color is not valid, black is returned
		 */
		static [[nodiscard]] sf::Color stringToColor(std::string str);

		/**
		 * \brief Takes a string of integer numbers separated with delimiter
		 * splits it to integer array and returns pointer to newly allocated
		 * array. Size of array is stored in size.
		 *
		 * \returns NULL on failure, valid pointer to int array otherwise
		 */
		static [[nodiscard]] std::optional<std::vector<int>> stringToIntArray(const char delimiter, const std::string &str);

		/**
		 * \brief Takes a string of integer numbers separated with delimiter
		 * and splits it to sf::Vector2i
		 *
		 * \details On failure, dst is not modified
		 *
		 * \returns TRUE if str contains only precisely two int numbers. FALSE otherwise
		 */
		static [[nodiscard]] std::optional<sf::Vector2i> stringToVector2i(const char delimiter, const std::string &str);

		/**
		* \brief Takes a string of integer numbers separated with delimiter
		* and splits it to sf::IntRect
		*
		* \details On failure, dst is not modified
		*
		* \returns TRUE if str contains precisely four int numbers. FALSE otherwise
		*/
		static [[nodiscard]] std::optional<sf::IntRect> stringToIntRect(const char delimiter, const std::string &str);

		/**
		 *  \brief Convert polar coordinates to cartesian
		 *
		 *  \param [in] angle 0° is at [0, 1], counting clockwise
		 *  \param [in] size Length of the vector
		 */
		static [[nodiscard]] sf::Vector2f polarToCartesian(float angle, float size) {
			const float PIOVER180 = 0.01745329252f;
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