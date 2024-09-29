#include <DGM/classes/Animation.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Animation]")
{
    SECTION("Default constructor works")
    {
        REQUIRE_NOTHROW([]() { dgm::Animation animation; }());
    }
}