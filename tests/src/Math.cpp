#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

using Catch::Approx;

#define NUMBER_DISTANCE(a, b) (std::max(a, b) - std::min(a, b))

#define COMPARE_VECTORS(v1, v2)                                                \
    REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f);                            \
    REQUIRE(NUMBER_DISTANCE(v1.y, v2.y) < 0.0001f);

TEST_CASE("[Math]")
{
    const auto ZERO_VECTOR = sf::Vector2f(0.f, 0.f);

    SECTION("getSize")
    {
        REQUIRE(dgm::Math::getSize({ 0.f, 0.f }) == 0.f);
        REQUIRE(dgm::Math::getSize({ 3.f, 4.f }) == 5.f);
        REQUIRE(dgm::Math::getSize({ 3.f, -4.f }) == 5.f);
    }

    SECTION("toUnit")
    {
        SECTION("Returns zero vector for input zero vector")
        {
            COMPARE_VECTORS(ZERO_VECTOR, dgm::Math::toUnit(ZERO_VECTOR));
        }

        SECTION("Returns vector with size of 1 and the same angle")
        {
            const auto vectors = std::vector { sf::Vector2f { 10.f, -3.f } };

            for (auto&& vec : vectors)
            {
                auto unit = dgm::Math::toUnit(vec);
                REQUIRE(Approx(dgm::Math::getSize(unit)) == 1.f);

                auto polar1 = dgm::Math::cartesianToPolar(vec);
                auto polar2 = dgm::Math::cartesianToPolar(unit);

                REQUIRE(polar1.angle == polar2.angle);
            }
        }
    }

    SECTION("rotateVector")
    {
        auto v1 = dgm::Math::getRotated({ 1.f, 0.f }, 90.f);
        auto v2 = dgm::Math::getRotated<dgm::Math::AngleType::Radians>(
            { 1.f, 0.f }, std::numbers::pi_v<float> / 2.f);

        COMPARE_VECTORS(v1, sf::Vector2f(0.f, 1.f));
        COMPARE_VECTORS(v2, sf::Vector2f(0.f, 1.f));
    }

    SECTION("getDotProduct")
    {
        const float dot =
            dgm::Math::getDotProduct({ -2.f, 4.f }, { 3.f, -4.f });
        REQUIRE(dot == -22.f);
    }

    SECTION("getRadialDifference")
    {
        REQUIRE(dgm::Math::getRadialDifference(0.f, 10.f) == 10.f);
        REQUIRE(dgm::Math::getRadialDifference(350.f, 0.f) == 10.f);
        REQUIRE(dgm::Math::getRadialDifference(270.f, 90.f) == 180.f);
        REQUIRE(dgm::Math::getRadialDifference(360.f, 0.f) == 0.f);
    }
}

TEST_CASE("Closest point on line", "Math")
{
    COMPARE_VECTORS(
        dgm::Math::getClosestPointOnLine(
            dgm::Math::Line(sf::Vector2f(1.f, 0.f), sf::Vector2f(0.f, 0.f)),
            sf::Vector2f(1.f, 1.f)),
        sf::Vector2f(1.f, 0.f));
    COMPARE_VECTORS(
        dgm::Math::getClosestPointOnLine(
            dgm::Math::Line(sf::Vector2f(-2.f, -2.f), sf::Vector2f(2.f, 2.f)),
            sf::Vector2f(-1.f, 1.f)),
        sf::Vector2f(0.f, 0.f));
}

TEST_CASE("Line intersection", "Math")
{
    using Line = dgm::Math::Line;
    using Point = sf::Vector2f;
    using Dir = sf::Vector2f;

    // Parallel with Y axis
    REQUIRE_FALSE(dgm::Math::getIntersection(
        Line(Dir(0.f, 1.f), Point(0.f, 0.f)),
        Line(Dir(0.f, 2.f), Point(10.f, 10.f))));

    // Parallel with X axis
    REQUIRE_FALSE(dgm::Math::getIntersection(
        Line(Dir(10.f, 0.f), Point(-1.f, 0.f)),
        Line(Dir(5.f, 0.f), Point(-10.f, 10.f))));

    // Simply parallel
    REQUIRE_FALSE(dgm::Math::getIntersection(
        Line(Dir(2.f, 2.f), Point(0.f, 0.f)),
        Line(Dir(5.f, 5.f), Point(0.f, 10.f))));

    // TODO: existing intersections
    // First line has a = 0
    auto v1 = dgm::Math::getIntersection(
                  Line(Dir(3.f, 0.f), Point(0.f, 0.f)),
                  Line(Dir(1.f, -1.f), Point(0.f, 3.f)))
                  .value();
    COMPARE_VECTORS(v1, Point(3.f, 0.f));

    // First line has b = 0
    auto v2 = dgm::Math::getIntersection(
                  Line(Dir(0.f, 2.f), Point(0.f, 0.f)),
                  Line(Dir(1.f, 1.f), Point(-2.f, 0.f)))
                  .value();
    COMPARE_VECTORS(v2, Point(0.f, 2.f));

    // First line has both a and b nonzero
    auto v3 = dgm::Math::getIntersection(
                  Line(Dir(1.f, 2.f), Point(-1.f, 0.f)),
                  Line(Dir(-3.f, 2.f), Point(4.f, -2.f)))
                  .value();
    COMPARE_VECTORS(v3, Point(-0.5f, 1.f));
}

TEST_CASE("Line with circle intersection", "[Math]")
{
    SECTION("No intersection")
    {
        auto&& points = dgm::Math::getIntersection(
            dgm::Math::Line(sf::Vector2f(100.f, 0.f), sf::Vector2f(0.f, 0.f)),
            dgm::Circle(100.f, 100.f, 10.f));
        REQUIRE_FALSE(points.has_value());
    }

    SECTION("Some intersetion")
    {
        auto&& points = dgm::Math::getIntersection(
            dgm::Math::Line(sf::Vector2f(10.f, 0.f), sf::Vector2f(0.f, 0.f)),
            dgm::Circle(100.f, 0.f, 10.f));
        REQUIRE(points.has_value());
        COMPARE_VECTORS(points->second, sf::Vector2f(90.f, 0.f));
        COMPARE_VECTORS(points->first, sf::Vector2f(110.f, 0.f));
    }
}

TEST_CASE("CartesianToPolar", "Conversion")
{
    sf::Vector2f vec;
    dgm::Math::PolarCoords refOut;

    SECTION("random case")
    {
        vec = sf::Vector2f(1.f, 1.f);
        refOut = dgm::Math::PolarCoords(45.f, sqrt(2.f));
    }

    SECTION("0 degrees")
    {
        vec = sf::Vector2f(5.f, 0.f);
        refOut = dgm::Math::PolarCoords(0.f, 5.f);
    }

    SECTION("90 degrees")
    {
        vec = sf::Vector2f(0.f, 10.f);
        refOut = dgm::Math::PolarCoords(90.f, 10.f);
    }

    SECTION("180 degrees")
    {
        vec = sf::Vector2f(-20.f, 0.f);
        refOut = dgm::Math::PolarCoords(180.f, 20.f);
    }

    SECTION("270 degrees")
    {
        vec = sf::Vector2f(0.f, -40.f);
        refOut = dgm::Math::PolarCoords(270.f, 40.f);
    }

    auto polar = dgm::Math::cartesianToPolar(vec);

    REQUIRE(Approx(polar.angle) == refOut.angle);
    REQUIRE(Approx(polar.length) == refOut.length);
}

TEST_CASE("PolarToCartesian", "Conversion")
{
    dgm::Math::PolarCoords polar(53.13f, 5.f);
    auto vec = dgm::Math::polarToCartesian(polar);

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

    outputVec =
        dgm::Math::polarToCartesian(dgm::Math::cartesianToPolar(inputVec));

    REQUIRE(Approx(inputVec.x) == outputVec.x);
    REQUIRE(Approx(inputVec.y) == outputVec.y);
}
