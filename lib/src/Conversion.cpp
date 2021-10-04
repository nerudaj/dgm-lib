#include <DGM\dgm.hpp>
#include <regex>
#include <algorithm>
#include <Strings.hpp>

const float PIOVER180 = 0.01745329252f;
const float _180OVERPI = 57.2957795131f;

using std::vector;
using std::string;
using dgm::Conversion;

uint8_t nibbleToUint8(const char bit) {
	if ('a' <= bit && bit <= 'f') return uint8_t(bit - 'a' + 10);
	return uint8_t(bit - '0');
}

uint8_t hexToUint8(const std::string &hex) {
	uint8_t result = 0;
	for (auto bit : hex) {
		result *= 16;
		result += nibbleToUint8(bit);
	}
	return result;
}

inline void uppercaseToLowercase(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

sf::Color Conversion::stringToColor(std::string str) {
	const std::regex hexaShort("#[0-9a-f]{3}");
	const std::regex hexaLong("#[0-9a-f]{6}");

	uppercaseToLowercase(str);
	
	uint8_t colorBits[3] = { 0 };
	
	if (std::regex_match(str, hexaShort)) {
		for (unsigned i = 1; i < str.size(); i++) {
			colorBits[i] = hexToUint8(str.substr(i, 1));
		}
	}
	else if (std::regex_match(str, hexaLong)) {
		for (unsigned i = 1, p = 0; i < str.size(); i += 2, p++) {
			colorBits[p] = hexToUint8(str.substr(i, 2));
		}
	}

	return sf::Color(colorBits[0], colorBits[1], colorBits[2]);
}

std::optional<vector<int>> Conversion::stringToIntArray(const char delimiter, const std::string & str) {
	vector<string> split;
	Strings::split(delimiter, str, split);
	
	vector<int> result(split.size());

	for (size_t i = 0; i < split.size(); i++) {
		try {
			result[i] = std::stoi(split[i]);
		} catch (...) {
			return std::nullopt;
		}
	}

	return result;
}

std::optional<sf::Vector2i> Conversion::stringToVector2i(const char delimiter, const std::string& str) {
	auto arr = Conversion::stringToIntArray(delimiter, str);
	if (!arr.has_value() || arr.value().size() != 2) return std::nullopt;
	return sf::Vector2i(arr.value()[0], arr.value()[1]);
}

std::optional<sf::IntRect> Conversion::stringToIntRect(const char delimiter, const std::string& str) {
	auto arr = Conversion::stringToIntArray(delimiter, str);
	if (!arr.has_value() || arr.value().size() != 4) return std::nullopt;
	return sf::IntRect(arr.value()[0], arr.value()[1], arr.value()[2], arr.value()[3]);
}

sf::Vector2f Conversion::cartesianToPolar(const float x, const float y) {
	float size = dgm::Math::vectorSize(x, y);

	     if (x == 0.f && y < 0.f)  return { 270.f, size };
	else if (x == 0.f && y >= 0.f) return {  90.f, size };
	else if (y == 0.f && x < 0.f)  return { 180.f, size };
	else if (y == 0.f && x >= 0.f) return {   0.f, size };

	return { std::atan2(y, x) * _180OVERPI, size };
}

