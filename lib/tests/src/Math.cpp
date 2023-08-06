#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

#define NUMBER_DISTANCE(a, b) (std::max(a, b) - std::min(a, b))

#define COMPARE_VECTORS(v1, v2)                                                \
    REQUIRE(NUMBER_DISTANCE(v1.x, v2.x) < 0.0001f);                            \
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