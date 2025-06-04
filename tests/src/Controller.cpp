#include <DGM/classes/Controller.hpp>
#include <catch2/catch_all.hpp>

enum class Action
{
    One,
    Two,
};

TEST_CASE("[Controller]")
{
    dgm::Controller<Action> input;

    SECTION("Can bind two distinct keyboard keys")
    {
        input.bindInput(Action::One, sf::Keyboard::Key::A);
        input.bindInput(Action::Two, sf::Keyboard::Key::B);
    }

    SECTION("Can bind two distinct xbox buttons")
    {
        input.bindInput(Action::One, 0);
        input.bindInput(Action::Two, 1);
    }

    SECTION(
        "BUG: Does not return false when mouse button is not bound and nothing "
        "is pressed")
    {
        input.bindInput(Action::One, sf::Keyboard::Key::A);
        REQUIRE_FALSE(input.readDigital(Action::One));
    }

    SECTION(
        "BUG: Does not return false when only gamepad input is bound and "
        "nothing is pressed")
    {
        input.bindInput(Action::One, 0);
        input.bindInput(
            Action::Two, sf::Joystick::Axis::PovX, dgm::AxisHalf::Negative);
        REQUIRE_FALSE(input.readDigital(Action::One));
        REQUIRE_FALSE(input.readDigital(Action::Two));
    }
}
