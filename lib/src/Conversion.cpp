#include <DGM/classes/Conversion.hpp>
#include <DGM/classes/Math.hpp>

using dgm::Conversion;

sf::Vector2f Conversion::cartesianToPolar(const float x, const float y) {
	float size = dgm::Math::vectorSize(x, y);

	     if (x == 0.f && y < 0.f)  return { 270.f, size };
	else if (x == 0.f && y >= 0.f) return {  90.f, size };
	else if (y == 0.f && x < 0.f)  return { 180.f, size };
	else if (y == 0.f && x >= 0.f) return {   0.f, size };

	return { std::atan2(y, x) * _180OVERPI, size };
}

