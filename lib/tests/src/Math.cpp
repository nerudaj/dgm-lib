#include <catch.hpp>
#include <DGM/dgm.hpp>

#define NUMBER_DISTANCE(a, b) \
	(std::max(a, b) - std::min(a, b))

#define COMPARE_VECTORS(v1, v2) \
	REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f); \
	REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

TEST_CASE("RotateVector", "Math")
{
	auto v = dgm::Math::rotateVector({ 1.f, 0.f }, 90.f);
	std::cout << v.x << ", " << v.y << std::endl;

	COMPARE_VECTORS(v, sf::Vector2f(0.f, 1.f));
}

TEST_CASE("VectorSize", "Math")
{
	REQUIRE(dgm::Math::vectorSize(0.f, 0.f) == 0.f);
	REQUIRE(dgm::Math::vectorSize(3.f, 4.f) == 5.f);
	REQUIRE(dgm::Math::vectorSize(-3.f, 4.f) == 5.f);
}

TEST_CASE("Radial difference", "Math")
{
	REQUIRE(dgm::Math::radialDifference(0.f, 10.f) == 10.f);
	REQUIRE(dgm::Math::radialDifference(350.f, 0.f) == 10.f);
	REQUIRE(dgm::Math::radialDifference(270.f, 90.f) == 180.f);
	REQUIRE(dgm::Math::radialDifference(360.f, 0.f) == 0.f);
}