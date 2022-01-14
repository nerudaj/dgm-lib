#pragma once

#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <DGM/classes/Traits.hpp>

namespace dgm {
	
	class Utility {
	public:
		template<class T>
		static std::string to_string(const sf::Vector2<T>& vec) {
			return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]";
		}

		template<IsSfVector T>
		struct less {
			[[nodiscard]] bool operator()(const T& a, const T& b) const {
				if (a.y == b.y) return a.x < b.x;
				return a.y < b.y;
			}
		};
	};
	
}
