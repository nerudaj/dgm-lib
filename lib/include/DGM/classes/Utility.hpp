#pragma once

#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <DGM/classes/Traits.hpp>

namespace dgm {
	
	class Utility {
	public:
		template<IsSfVector T>
		static std::string to_string(const T& vec) {
			return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + "]";
		}

		template<IsSfVector T>
		struct less {
			[[nodiscard]] bool operator()(const T& a, const T& b) const {
				return a.y == b.y
					? a.x < b.x
					: a.y < b.y;
			}
		};
	};
}
