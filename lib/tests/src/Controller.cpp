#include <catch.hpp>
#include <DGM/classes/Controller.hpp>

TEST_CASE("[Controller]")
{
	dgm::Controller input;

	SECTION("Can bind two distinct keyboard keys")
	{
		input.bindInput(0, sf::Keyboard::A);
		input.bindInput(1, sf::Keyboard::B);
	}

	SECTION("Can bind two distinct xbox buttons")
	{
		input.bindInput(0, dgm::Xbox::Button::A);
		input.bindInput(1, dgm::Xbox::Button::B);
	}

	SECTION("Can bind all possible inputs to the same code")
	{
		input.bindInput(0, dgm::Xbox::Button::A);
		input.bindInput(0, sf::Keyboard::A);
		input.bindInput(0, sf::Mouse::Button::Left);
		input.bindInput(0, dgm::Xbox::Axis::LTrigger);
	}

	SECTION("BUG: Does not return true when mouse button is not bound")
	{
		input.bindInput(0, sf::Keyboard::A);
		REQUIRE_FALSE(input.getValue(0));
	}
}
