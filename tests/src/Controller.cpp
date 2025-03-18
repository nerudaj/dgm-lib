#include <DGM/classes/Controller.hpp>
#include <catch2/catch_all.hpp>

enum class Action
{
    One,
    Two,
};

struct ApplyBind
{
    void operator()(unsigned idx)
    {
        input.bindInput(Action::One, idx);
    }

    void operator()(std::pair<sf::Joystick::Axis, dgm::AxisHalf> val)
    {
        input.bindInput(Action::Two, val.first, val.second);
    }

    dgm::Controller<Action>& input;
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

    SECTION("Can bind all possible inputs to the same code")
    {
        std::visit(
            ApplyBind(input),
            dgm::translateGamepadCode(
                dgm::GamepadCode::A,
                sf::Joystick::Identification { .vendorId = 0x045E,
                                               .productId = 0x02FF }));

        input.bindInput(Action::One, sf::Keyboard::Key::A);
        input.bindInput(
            Action::One,
            sf::Mouse::Button::Left,
            dgm::DigitalReadKind::OnPress);
        input.bindInput(
            Action::One, sf::Joystick::Axis::PovX, dgm::AxisHalf::Negative);
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
