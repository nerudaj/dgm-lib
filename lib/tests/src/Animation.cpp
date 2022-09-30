#include <catch.hpp>
#include <DGM/classes/Animation.hpp>

TEST_CASE("Animation", "[Animation]")
{
	SECTION("Default constructor works")
	{
		REQUIRE_NOTHROW([] ()
		{
			dgm::Animation animation;
		} ());
	}
}