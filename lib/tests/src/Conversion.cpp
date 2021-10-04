#include <catch.hpp>
#include <DGM/dgm.hpp>

TEST_CASE("CartesianToPolar", "Conversion") {
	sf::Vector2f vec;
	sf::Vector2f refOut;

	SECTION("random case") {
		vec = sf::Vector2f(1.f, 1.f);
		refOut = sf::Vector2f(45.f, sqrt(2.f));
	}

	SECTION("0 degrees") {
		vec = sf::Vector2f(5.f, 0.f);
		refOut = sf::Vector2f(0.f, 5.f);
	}

	SECTION("90 degrees") {
		vec = sf::Vector2f(0.f, 10.f);
		refOut = sf::Vector2f(90.f, 10.f);
	}

	SECTION("180 degrees") {
		vec = sf::Vector2f(-20.f, 0.f);
		refOut = sf::Vector2f(180.f, 20.f);
	}

	SECTION("270 degrees") {
		vec = sf::Vector2f(0.f, -40.f);
		refOut = sf::Vector2f(270.f, 40.f);
	}

	auto polar = dgm::Conversion::cartesianToPolar(vec);

	REQUIRE(Approx(polar.x) == refOut.x);
	REQUIRE(Approx(polar.y) == refOut.y);
}

TEST_CASE("PolarToCartesian", "Conversion") {
	sf::Vector2f polar(53.13f, 5.f);
	auto vec = dgm::Conversion::polarToCartesian(polar);

	REQUIRE(Approx(vec.x) == 3.f);
	REQUIRE(Approx(vec.y) == 4.f);
}

TEST_CASE("PolarCartesianSymmetry", "Conversion") {
	sf::Vector2f inputVec;
	sf::Vector2f outputVec;

	SECTION("+x+y") {
		inputVec = sf::Vector2f(100.f, 20.f);
	}

	SECTION("-x+y") {
		inputVec = sf::Vector2f(-55.f, 12.f);
	}

	SECTION("-x-y") {
		inputVec = sf::Vector2f(-70.f, -22.f);
	}

	SECTION("+x-y") {
		inputVec = sf::Vector2f(255.f, -2.f);
	}

	outputVec = dgm::Conversion::polarToCartesian(
		dgm::Conversion::cartesianToPolar(inputVec));

	REQUIRE(Approx(inputVec.x) == outputVec.x);
	REQUIRE(Approx(inputVec.y) == outputVec.y);
}

TEST_CASE("StringToColor", "Conversion") {
	auto c1 = dgm::Conversion::stringToColor("#F0F");
	auto c2 = dgm::Conversion::stringToColor("#FF00FF");
	auto c3 = dgm::Conversion::stringToColor("blah");

	REQUIRE(c1 == sf::Color(255, 0, 255));
	REQUIRE(c2 == sf::Color(255, 0, 255));
	REQUIRE(c3 == sf::Color::Black);
}

TEST_CASE("StringToIntArray", "Conversion") {
	SECTION("empty array") {
		auto arr = dgm::Conversion::stringToIntArray(',', "");
		REQUIRE(arr.has_value());
		REQUIRE(arr.value().size() == 0);
	}

	SECTION("invalid array") {
		auto arr = dgm::Conversion::stringToIntArray(',', "10, bug");
		REQUIRE(!arr.has_value());
	}

	// TODO: too big number
	// TODO: not entirely one number
	// TODO: no conversion

	SECTION("valid array") {
		auto arr = dgm::Conversion::stringToIntArray(',', "10, 20, 30");

		REQUIRE(arr.has_value());
		REQUIRE(arr.value().size() == 3);
		REQUIRE(arr.value()[0] == 10);
		REQUIRE(arr.value()[1] == 20);
		REQUIRE(arr.value()[2] == 30);
	}
}

TEST_CASE("StringToIntRect", "Conversion") {
	SECTION("Valid conversion") {
		auto v = dgm::Conversion::stringToIntRect(':', "10:20:30:40");

		REQUIRE(v.has_value());
		REQUIRE(v.value().left == 10);
		REQUIRE(v.value().top == 20);
		REQUIRE(v.value().width == 30);
		REQUIRE(v.value().height == 40);
	}

	SECTION("Invalid conversion - bad number of ints") {
		auto v1 = dgm::Conversion::stringToIntRect(',', "10,20,30,40,50");
		REQUIRE(!v1.has_value());

		auto v2 = dgm::Conversion::stringToIntRect(',', "10,20,30");
		REQUIRE(!v2.has_value());
	}

	SECTION("Invalid conversion - invalid numbers") {
		auto v = dgm::Conversion::stringToIntRect(',', "10,a,30,b");
		REQUIRE(!v.has_value());
	}
}

TEST_CASE("StringToVector2i", "Conversion") {
	SECTION("Valid conversion") {
		auto v = dgm::Conversion::stringToVector2i(':', "10:20");

		REQUIRE(v.has_value());
		REQUIRE(v.value().x == 10);
		REQUIRE(v.value().y == 20);
	}

	SECTION("Invalid conversion - bad number of ints") {
		auto v1 = dgm::Conversion::stringToVector2i(',', "10");
		REQUIRE(!v1.has_value());

		auto v2 = dgm::Conversion::stringToVector2i(',', "10,20,30");
		REQUIRE(!v2.has_value());
	}

	SECTION("Invalid conversion - invalid numbers") {
		auto v1 = dgm::Conversion::stringToVector2i(',', "10,a");
		REQUIRE(!v1.has_value());
	}
}