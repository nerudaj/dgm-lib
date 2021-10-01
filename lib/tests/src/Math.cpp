#include <catch.hpp>
#include <DGM/dgm.hpp>

#define NUMBER_DISTANCE(a, b) \
	(std::max(a, b) - std::min(a, b))

#define COMPARE_VECTORS(v1, v2) \
	REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f); \
	REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

TEST_CASE("Clamp", "Math") {
	SECTION("int version") {
		REQUIRE(dgm::Math::clamp(-1, 0, 2) == 0);
		REQUIRE(dgm::Math::clamp(1, 0, 2) == 1);
		REQUIRE(dgm::Math::clamp(3, 0, 2) == 2);
	}

	SECTION("float version") {
		REQUIRE(dgm::Math::clamp(-1.f, 0.f, 2.f) == 0.f);
		REQUIRE(dgm::Math::clamp(1.f, 0.f, 2.f) == 1.f);
		REQUIRE(dgm::Math::clamp(3.f, 0.f, 2.f) == 2.f);
	}
}

TEST_CASE("Lerp", "Math") {
	REQUIRE(dgm::Math::lerp(0.f, 1.f, 0.f) == 0.f);
	REQUIRE(dgm::Math::lerp(0.f, 1.f, 1.f) == 1.f);
	REQUIRE(dgm::Math::lerp(0.f, 1.f, 0.25f) == 0.25f);
}

TEST_CASE("RotateVector", "Math") {
	auto v = dgm::Math::rotateVector({ 1.f, 0.f }, 90.f);
	std::cout << v.x << ", " << v.y << std::endl;

	COMPARE_VECTORS(v, sf::Vector2f(0.f, 1.f));
}

TEST_CASE("VectorSize", "Math") {
	REQUIRE(dgm::Math::vectorSize(0.f, 0.f) == 0.f);
	REQUIRE(dgm::Math::vectorSize(3.f, 4.f) == 5.f);
	REQUIRE(dgm::Math::vectorSize(-3.f, 4.f) == 5.f);
}
