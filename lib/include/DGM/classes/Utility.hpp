#pragma once

#include <iostream>
#include <SFML/System/Vector2.hpp>

namespace dgm {
	
	class Utility {
	public:
		template<class T>
		static std::string to_string(const sf::Vector2<T>& vec) {
			return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]";
		}
	};
	
}
