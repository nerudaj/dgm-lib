#include <DGM/classes/Animation.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Animation]")
{
    SECTION("Default constructor works")
    {
        REQUIRE_NOTHROW([]() { dgm::Animation animation; }());
    }

    SECTION("set/getSpeed works")
    {
        dgm::Animation animation;
        animation.setSpeed(42);
        REQUIRE(animation.getSpeed() == 42);
    }
}