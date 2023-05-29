#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>

using Catch::Approx;

TEST_CASE("CartesianToPolar", "Conversion")
{
    sf::Vector2f vec;
    sf::Vector2f refOut;

    SECTION("random case")
    {
        vec = sf::Vector2f(1.f, 1.f);
        refOut = sf::Vector2f(45.f, sqrt(2.f));
    }

    SECTION("0 degrees")
    {
        vec = sf::Vector2f(5.f, 0.f);
        refOut = sf::Vector2f(0.f, 5.f);
    }

    SECTION("90 degrees")
    {
        vec = sf::Vector2f(0.f, 10.f);
        refOut = sf::Vector2f(90.f, 10.f);
    }

    SECTION("180 degrees")
    {
        vec = sf::Vector2f(-20.f, 0.f);
        refOut = sf::Vector2f(180.f, 20.f);
    }

    SECTION("270 degrees")
    {
        vec = sf::Vector2f(0.f, -40.f);
        refOut = sf::Vector2f(270.f, 40.f);
    }

    auto polar = dgm::Conversion::cartesianToPolar(vec);

    REQUIRE(Approx(polar.x) == refOut.x);
    REQUIRE(Approx(polar.y) == refOut.y);
}

TEST_CASE("PolarToCartesian", "Conversion")
{
    sf::Vector2f polar(53.13f, 5.f);
    auto vec = dgm::Conversion::polarToCartesian(polar);

    REQUIRE(Approx(vec.x) == 3.f);
    REQUIRE(Approx(vec.y) == 4.f);
}

TEST_CASE("PolarCartesianSymmetry", "Conversion")
{
    sf::Vector2f inputVec;
    sf::Vector2f outputVec;

    SECTION("+x+y")
    {
        inputVec = sf::Vector2f(100.f, 20.f);
    }

    SECTION("-x+y")
    {
        inputVec = sf::Vector2f(-55.f, 12.f);
    }

    SECTION("-x-y")
    {
        inputVec = sf::Vector2f(-70.f, -22.f);
    }

    SECTION("+x-y")
    {
        inputVec = sf::Vector2f(255.f, -2.f);
    }

    outputVec = dgm::Conversion::polarToCartesian(
        dgm::Conversion::cartesianToPolar(inputVec));

    REQUIRE(Approx(inputVec.x) == outputVec.x);
    REQUIRE(Approx(inputVec.y) == outputVec.y);
}
