#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Rect-to-point", "Collision")
{
    dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

    SECTION("No collision")
    {
        sf::Vector2i point(-1, -1);
        REQUIRE_FALSE(dgm::Collision::basic(rect, point));
    }

    SECTION("Normal collision")
    {
        sf::Vector2i point(5, 5);
        REQUIRE(dgm::Collision::basic(rect, point));
    }
}

TEST_CASE("Rect-to-rect", "Collision")
{
    dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

    SECTION("No collision")
    {
        dgm::Rect rect2(-20.f, -20.f, 10.f, 10.f);
        REQUIRE_FALSE(dgm::Collision::basic(rect, rect2));
    }

    SECTION("Normal collision")
    {
        dgm::Rect rect2(-5.f, -5.f, 10.f, 10.f);
        REQUIRE(dgm::Collision::basic(rect, rect2));
    }

    SECTION("Almost touching")
    {
        dgm::Rect rect2(-5.f, -5.f, 4.9f, 4.9f);
        REQUIRE_FALSE(dgm::Collision::basic(rect, rect2));
    }
}

TEST_CASE("Rect-to-circle", "Collision")
{
    dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

    SECTION("No collision")
    {
        dgm::Circle circle(-20.f, -20.f, 10.f);
        REQUIRE_FALSE(dgm::Collision::basic(rect, circle));
    }

    SECTION("Normal collision")
    {
        dgm::Circle circle(-5.f, -5.f, 10.f);
        REQUIRE(dgm::Collision::basic(rect, circle));
    }
}

TEST_CASE("Circle-to-circle", "Collision")
{
    dgm::Circle circle1(5.f, 5.f, 5.f);

    SECTION("No collision")
    {
        dgm::Circle circle2(-5.f, -5.f, 5.f);
        REQUIRE_FALSE(dgm::Collision::basic(circle1, circle2));
    }

    SECTION("Normal collision")
    {
        dgm::Circle circle2(0.f, 0.f, 5.f);
        REQUIRE(dgm::Collision::basic(circle1, circle2));
    }
}

TEST_CASE("Circle-to-point", "Collision")
{
    dgm::Circle circle(5.f, 5.f, 5.f);

    SECTION("No collision")
    {
        sf::Vector2i point(0, 0);
        REQUIRE_FALSE(dgm::Collision::basic(circle, point));
    }

    SECTION("Normal collision")
    {
        sf::Vector2i point(4, 4);
        REQUIRE(dgm::Collision::basic(circle, point));
    }
}

TEST_CASE("Mesh-to-rect", "Collision")
{
    auto&& mesh = dgm::Mesh(
        std::vector<bool> { 1, 0, 1, 1 },
        sf::Vector2u(2u, 2u),
        sf::Vector2u(10u, 10u));

    SECTION("No collision")
    {
        dgm::Rect rect(11.f, 1.f, 8.f, 8.f);
        std::size_t hitPos = 0;
        REQUIRE_FALSE(dgm::Collision::basic(mesh, rect, &hitPos));
        REQUIRE(hitPos == 0);
    }

    SECTION("Normal collision")
    {
        dgm::Rect rect(11.f, 11.f, 8.f, 8.f);
        std::size_t hitPos = 0;
        REQUIRE(dgm::Collision::basic(mesh, rect, &hitPos));
        REQUIRE(hitPos == 3);
    }
}

TEST_CASE("Mesh-to-circle", "Collision")
{
    auto&& mesh = dgm::Mesh(
        std::vector<bool> { 1, 0, 1, 1 },
        sf::Vector2u(2u, 2u),
        sf::Vector2u(10u, 10u));

    SECTION("No collision")
    {
        dgm::Circle circle(15.f, 5.f, 4.f);
        std::size_t hitPos = 0;
        REQUIRE_FALSE(dgm::Collision::basic(mesh, circle, &hitPos));
        REQUIRE(hitPos == 0);
    }

    SECTION("Normal collision")
    {
        dgm::Circle circle(15.f, 10.f, 4.f);
        std::size_t hitPos = 0;
        REQUIRE(dgm::Collision::basic(mesh, circle, &hitPos));
        REQUIRE(hitPos == 3);
    }
}

TEST_CASE("Circle-to-cone", "Collision")
{
    SECTION("Catches collision when circle lies on the forward line of cone")
    {
        dgm::VisionCone cone(100.f, 50.f);
        dgm::Circle circle(0.f, 0.f, 20.f);

        cone.setPosition(100.f, 100.f);

        SECTION("Forward follows X axis")
        {
            SECTION("Circle is behind the cone")
            {
                circle.setPosition(50.f, 100.f);
                REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is fully inside the cone")
            {
                circle.setPosition(150.f, 100.f);
                REQUIRE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is partially inside the cone")
            {
                circle.setPosition(215.f, 100.f);
                REQUIRE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is front of the cone")
            {
                circle.setPosition(250.f, 100.f);
                REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
            }
        }

        SECTION("Forward is rotated")
        {
            cone.setRotation(45.f);

            SECTION("Circle is behind the cone")
            {
                circle.setPosition(50.f, 50.f);
                REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is fully inside the cone")
            {
                circle.setPosition(125.f, 125.f);
                REQUIRE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is partially inside the cone")
            {
                circle.setPosition(150.f, 150.f);
                REQUIRE(dgm::Collision::basic(circle, cone));
            }

            SECTION("Circle is front of the cone")
            {
                circle.setPosition(250.f, 250.f);
                REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
            }
        }
    }

    SECTION("When circle is on the right edge of the cone")
    {
        // Center.x of the circle is > cone.lenght, but center.x - radius <
        // cone.length
        dgm::VisionCone cone(100.f, 50.f);
        dgm::Circle circle(0.f, 0.f, 20.f);
        cone.setPosition(100.f, 100.f);

        SECTION("Near enough to have collision")
        {
            circle.setPosition(95.f, 90.f);
            REQUIRE(dgm::Collision::basic(circle, cone));
        }

        SECTION("Too far")
        {
            circle.setPosition(95.f, 80.f);
            REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
        }
    }

    SECTION("When circle is on the left edge of the cone")
    {
        // Center.x of the circle is < 0, but center.x + radius > 0
        dgm::VisionCone cone(100.f, 50.f);
        dgm::Circle circle(0.f, 0.f, 20.f);
        cone.setPosition(100.f, 100.f);

        SECTION("Near enough to have collision")
        {
            circle.setPosition(205.f, 90.f);
            REQUIRE(dgm::Collision::basic(circle, cone));
        }

        SECTION("Too far")
        {
            circle.setPosition(205.f, 50.f);
            REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
        }
    }

    SECTION("When circle is somewhere next to the cone")
    {
        dgm::VisionCone cone(100.f, 50.f);
        dgm::Circle circle(0.f, 0.f, 20.f);
        cone.setPosition(100.f, 100.f);

        SECTION("Far from cone")
        {
            circle.setPosition(150.f, 0.f);
            REQUIRE_FALSE(dgm::Collision::basic(circle, cone));
        }

        SECTION("Above the cone, colliding")
        {
            circle.setPosition(150.f, 80.f);
            REQUIRE(dgm::Collision::basic(circle, cone));
        }

        SECTION("Under the cone, colliding")
        {
            circle.setPosition(150.f, 120.f);
            REQUIRE(dgm::Collision::basic(circle, cone));
        }
    }
}

TEST_CASE("Point-to-cone", "Collision")
{
    dgm::VisionCone cone(20.f, 50.f);
    cone.setPosition(20.f, 20.f);

    SECTION("Point is behind origin")
    {
        sf::Vector2i point(0, 20);
        REQUIRE_FALSE(dgm::Collision::basic(point, cone));
    }

    SECTION("Point is atop of cone")
    {
        sf::Vector2i point(30, -60);
        REQUIRE_FALSE(dgm::Collision::basic(point, cone));
    }

    SECTION("Point is front of cone")
    {
        sf::Vector2i point(45, 20);
        REQUIRE_FALSE(dgm::Collision::basic(point, cone));
    }

    SECTION("Point is under cone")
    {
        sf::Vector2i point(30, 60);
        REQUIRE_FALSE(dgm::Collision::basic(point, cone));
    }

    SECTION("Point is inside cone")
    {
        sf::Vector2i point(30, 30);
        REQUIRE(dgm::Collision::basic(point, cone));
    }
}
