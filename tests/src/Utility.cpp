#include "TestDataDir.hpp"
#include <DGM/classes/Utility.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Vector to string", "[Utility]")
{
    REQUIRE(dgm::Utility::to_string(sf::Vector2u(42, 69)) == "[42, 69]");
}

TEST_CASE("Vector less implementation", "[Utility]")
{
    using less = dgm::Utility::less<sf::Vector2u>;

    REQUIRE_FALSE(less {}(sf::Vector2u(1, 1), sf::Vector2u(1, 1)));

    REQUIRE(less {}(sf::Vector2u(1, 0), sf::Vector2u(1, 1)));
    REQUIRE_FALSE(less {}(sf::Vector2u(1, 1), sf::Vector2u(1, 0)));

    REQUIRE(less {}(sf::Vector2u(0, 1), sf::Vector2u(1, 1)));
    REQUIRE_FALSE(less {}(sf::Vector2u(1, 1), sf::Vector2u(0, 1)));

    REQUIRE(less {}(sf::Vector2u(2, 1), sf::Vector2u(1, 2)));
    REQUIRE_FALSE(less {}(sf::Vector2u(1, 2), sf::Vector2u(2, 1)));
}

TEST_CASE("loadFileAllText", "[Utility]")
{
    SECTION("load from stream")
    {
        std::stringstream str("hello");
        auto result = dgm::Utility::loadFileAllText(str);
        REQUIRE(result.has_value());
        REQUIRE(result.value() == "hello");
    }

    SECTION("load from disc")
    {
        auto result = dgm::Utility::loadFileAllText(
            TEST_DATA_DIR + "/loadFileAllText.txt");
        REQUIRE(result.has_value());
        REQUIRE(result.value() == "hello");
    }
}