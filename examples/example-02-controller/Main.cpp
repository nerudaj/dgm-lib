/*
An example showing how to bind keyboard, mouse and xbox to
abstract inputs.
*/

#include <DGM/dgm.hpp>

// Each physical input needs to be mapped to
// some abstract action represented as a value
// in this enum.
enum class Action
{
    L_Up,
    L_Left,
    L_Down,
    L_Right,
    R_Up,
    R_Left,
    R_Down,
    R_Right,
    A,
    B,
    X,
    Y,
    Back,
    Start,
    LBumper,
    RBumper,
    LTrigger,
    RTrigger,
    LStick,
    RStick
};

enum class [[nodiscard]] GamepadCode
{
    A,
    B,
    X,
    Y,
    Start,
    Select,
    Capture,
    LBumper,
    RBumper,
    LTrigger,
    RTrigger,
    DPadLeft,
    DPadRight,
    DPadUp,
    DPadDown,
    LStickLeft,
    LStickRight,
    LStickUp,
    LStickDown,
    LStickPress,
    RStickLeft,
    RStickRight,
    RStickUp,
    RStickDown,
    RStickPress,
};

using SfmlGamepadInput =
    std::variant<unsigned int, std::pair<sf::Joystick::Axis, dgm::AxisHalf>>;

/**
 * SFML uses abstract mapping codes because each gamepad is different
 * and each gamepad low-level API is different.
 *
 * In your game, either let the user calibrate inputs themselves, or you
 * need to come up with a mapping function such as the following one.
 *
 * You can use SDL gamepad database, it reportedly contains vendorID and
 * productID in the GUID, but their mapping for Dpads is weird.
 */
SfmlGamepadInput translateGamepadCode(
    GamepadCode code, const sf::Joystick::Identification& identity)
{
    if (identity.vendorId == 0x045E) // Microsoft
    {
        if (identity.productId == 0x02FF) // Controller (Xbox One For Windows)
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 0u;
            case B:
                return 1u;
            case X:
                return 2u;
            case Y:
                return 3u;
            case Start:
                return 7u;
            case Select:
                return 6u;
            case Capture:
                return 12u;
            case LBumper:
                return 4u;
            case RBumper:
                return 5u;
            case LTrigger:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Positive };
            case RTrigger:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Negative };
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Positive };
            case LStickPress:
                return 8u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::U,
                                   dgm::AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::U,
                                   dgm::AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::V,
                                   dgm::AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::V,
                                   dgm::AxisHalf::Positive };
            case RStickPress:
                return 14u;
            }
        }
    }
    else if (identity.vendorId == 0x358A) // Backbone
    {
        if (identity.productId == 0x0)
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 4u;
            case B:
                return 5u;
            case X:
                return 7u;
            case Y:
                return 8u;
            case Start:
                return 15u;
            case Select:
                return 14u;
            case Capture:
                return 0u; // not a real button, unused
            case LBumper:
                return 10u;
            case RBumper:
                return 11u;
            case LTrigger:
                return 12u;
            case RTrigger:
                return 13u;
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Positive };
            case LStickPress:
                return 17u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::R,
                                   dgm::AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::R,
                                   dgm::AxisHalf::Positive };
            case RStickPress:
                return 18u;
            }
        }
    }
    else if (identity.vendorId == 0x0079) // DragonRise Inc. / Canyon?
    {
        if (identity.productId == 0x0304) // Canyon controller GPW3
        {
            switch (code)
            {
                using enum GamepadCode;
            case A:
                return 0u;
            case B:
                return 1u;
            case X:
                return 3u;
            case Y:
                return 4u;
            case Start:
                return 11u;
            case Select:
                return 10u;
            case Capture:
                return 12u;
            case LBumper:
                return 6u;
            case RBumper:
                return 7u;
            case LTrigger:
                return 8u;
            case RTrigger:
                return 9u;
            case DPadLeft:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Negative };
            case DPadRight:
                return std::pair { sf::Joystick::Axis::PovX,
                                   dgm::AxisHalf::Positive };
            case DPadUp:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Positive };
            case DPadDown:
                return std::pair { sf::Joystick::Axis::PovY,
                                   dgm::AxisHalf::Negative };
            case LStickLeft:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Negative };
            case LStickRight:
                return std::pair { sf::Joystick::Axis::X,
                                   dgm::AxisHalf::Positive };
            case LStickUp:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Negative };
            case LStickDown:
                return std::pair { sf::Joystick::Axis::Y,
                                   dgm::AxisHalf::Positive };
            case LStickPress:
                return 13u;
            case RStickLeft:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Negative };
            case RStickRight:
                return std::pair { sf::Joystick::Axis::Z,
                                   dgm::AxisHalf::Positive };
            case RStickUp:
                return std::pair { sf::Joystick::Axis::R,
                                   dgm::AxisHalf::Negative };
            case RStickDown:
                return std::pair { sf::Joystick::Axis::R,
                                   dgm::AxisHalf::Positive };
            case RStickPress:
                return 14u;
            }
        }
    }

    throw dgm::Exception("Unrecognized gamepad: " + identity.name);
}

template<class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};

#ifdef ANDROID
// Deduction guide not needed since C++20
template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;
#endif

int main()
{
    dgm::Window window({ 1280, 720 }, "Example 02: Controller", false);

    dgm::ResourceManager resmgr;
    std::ignore = resmgr.loadResourcesFromDirectory<sf::Font>(
        RESOURCE_DIR, dgm::Utility::loadFont, { ".ttf" });

    // After creating the controller class, you need to bind
    // physical inputs to abstract actions
    // A single physical input can be bound to multiple actions
    // and a multiple physical inputs can be bound to a single action
    dgm::Controller<Action> input;

    // IMPORTANT!
    // When an action is associated with just a key, it will return
    // either 0 or 1 with getInputValue
    // But when the action is also associated with negative analog
    // inputs of a gamepad (left trigger, analog sticks)
    // it will return -1 or 0 so you can just sum left and right
    // values to get the final vector.
    // see getDirection lambda later in this example

    using Key = sf::Keyboard::Key;

    // Bind keyboard
    input.bindInput(Action::L_Up, Key::Up);
    input.bindInput(Action::L_Left, Key::Left);
    input.bindInput(Action::L_Down, Key::Down);
    input.bindInput(Action::L_Right, Key::Right);
    input.bindInput(Action::A, Key::S);
    input.bindInput(Action::B, Key::D);
    input.bindInput(Action::X, Key::A);
    input.bindInput(Action::Y, Key::W);
    input.bindInput(Action::Back, Key::B);
    input.bindInput(Action::Start, Key::N);

    try
    {
        auto bindInput = [&](Action code, auto binding)
        {
            std::visit(
                overloads {
                    [&](unsigned idx) { input.bindInput(code, idx); },
                    [&](const std::pair<sf::Joystick::Axis, dgm::AxisHalf>&
                            pair)
                    { input.bindInput(code, pair.first, pair.second); },
                },
                binding);
        };

        const auto id = sf::Joystick::getIdentification(0);

        // Bind controller
        bindInput(Action::L_Up, translateGamepadCode(GamepadCode::DPadUp, id));
        bindInput(
            Action::L_Left, translateGamepadCode(GamepadCode::DPadLeft, id));
        bindInput(
            Action::L_Down, translateGamepadCode(GamepadCode::DPadDown, id));
        bindInput(
            Action::L_Right, translateGamepadCode(GamepadCode::DPadRight, id));
        bindInput(
            Action::L_Up, translateGamepadCode(GamepadCode::LStickUp, id));
        bindInput(
            Action::L_Left, translateGamepadCode(GamepadCode::LStickLeft, id));
        bindInput(
            Action::L_Down, translateGamepadCode(GamepadCode::LStickDown, id));
        bindInput(
            Action::L_Right,
            translateGamepadCode(GamepadCode::LStickRight, id));
        bindInput(
            Action::R_Up, translateGamepadCode(GamepadCode::RStickUp, id));
        bindInput(
            Action::R_Left, translateGamepadCode(GamepadCode::RStickLeft, id));
        bindInput(
            Action::R_Down, translateGamepadCode(GamepadCode::RStickDown, id));
        bindInput(
            Action::R_Right,
            translateGamepadCode(GamepadCode::RStickRight, id));
        bindInput(Action::A, translateGamepadCode(GamepadCode::A, id));
        bindInput(Action::B, translateGamepadCode(GamepadCode::B, id));
        bindInput(Action::X, translateGamepadCode(GamepadCode::X, id));
        bindInput(Action::Y, translateGamepadCode(GamepadCode::Y, id));
        bindInput(Action::Back, translateGamepadCode(GamepadCode::Select, id));
        bindInput(Action::Start, translateGamepadCode(GamepadCode::Start, id));
        bindInput(
            Action::LBumper, translateGamepadCode(GamepadCode::LBumper, id));
        bindInput(
            Action::RBumper, translateGamepadCode(GamepadCode::RBumper, id));
        bindInput(
            Action::LTrigger, translateGamepadCode(GamepadCode::LTrigger, id));
        bindInput(
            Action::RTrigger, translateGamepadCode(GamepadCode::RTrigger, id));
        bindInput(
            Action::LStick, translateGamepadCode(GamepadCode::LStickPress, id));
        bindInput(
            Action::RStick, translateGamepadCode(GamepadCode::RStickPress, id));
    }
    catch (...)
    {
    }

    // Bind mouse
    input.bindInput(Action::LTrigger, sf::Mouse::Button::Left);
    input.bindInput(Action::RTrigger, sf::Mouse::Button::Right);

    sf::RectangleShape dpadUp, dpadLeft, dpadDown, dpadRight;
    dpadUp.setSize({ 20.f, 50.f });
    dpadUp.setOrigin({ 10.f, 50.f });
    dpadUp.setPosition({ 213.f, 360.f });
    dpadLeft.setSize({ 50.f, 20.f });
    dpadLeft.setOrigin({ 50.f, 10.f });
    dpadLeft.setPosition({ 213.f, 360.f });
    dpadDown.setSize({ 20.f, 50.f });
    dpadDown.setOrigin({ 10.f, 0.f });
    dpadDown.setPosition({ 213.f, 360.f });
    dpadRight.setSize({ 50.f, 20.f });
    dpadRight.setOrigin({ 0.f, 10.f });
    dpadRight.setPosition({ 213.f, 360.f });

    sf::RectangleShape back, start;
    back.setSize({ 50.f, 30.f });
    back.setOrigin({ 0.f, 15.f });
    back.setPosition({ 426.f, 360.f });
    start.setSize({ 50.f, 30.f });
    start.setOrigin({ 50.f, 15.f });
    start.setPosition({ 853.f, 360.f });

    sf::RectangleShape lbumper, rbumper;
    lbumper.setSize({ 60.f, 30.f });
    lbumper.setOrigin({ 30.f, 15.f });
    rbumper = lbumper;

    lbumper.setPosition({ 213.f, 240.f });
    rbumper.setPosition({ 1066.f, 240.f });

    sf::CircleShape a, b, x, y;
    a.setRadius(15.f);
    a.setOrigin({ 15.f, 15.f });
    b = a;
    x = a;
    y = a;

    a.setPosition({ 1066.f, 390.f });
    b.setPosition({ 1096.f, 360.f });
    x.setPosition({ 1036.f, 360.f });
    y.setPosition({ 1066.f, 330.f });

    sf::RectangleShape ltrigger, rtrigger;
    ltrigger.setOrigin({ 10.f, 100.f });
    ltrigger.setPosition({ 213.f, 120.f });
    rtrigger.setOrigin({ 10.f, 100.f });
    rtrigger.setPosition({ 1066.f, 120.f });

    sf::CircleShape lStickOutline, rStickOutline;
    lStickOutline.setFillColor(sf::Color::Transparent);
    lStickOutline.setOutlineColor(sf::Color::White);
    lStickOutline.setOutlineThickness(1.f);
    lStickOutline.setRadius(100.f);
    lStickOutline.setOrigin({ 100.f, 100.f });

    rStickOutline = lStickOutline;
    lStickOutline.setPosition({ 426.f, 600.f });
    rStickOutline.setPosition({ 853.f, 600.f });

    sf::CircleShape lStickHat, rStickHat;
    lStickHat.setRadius(2.5f);
    lStickHat.setOrigin({ 2.5f, 2.5f });
    rStickHat = lStickHat;

    auto setFillColor = [&input](sf::Shape& shape, Action action)
    {
        shape.setFillColor(
            input.readDigital(action) ? sf::Color::Green : sf::Color::Red);
    };

    auto getDirection =
        [&input](
            Action up, Action down, Action left, Action right) -> sf::Vector2f
    {
        return sf::Vector2f(
                   -input.readAnalog(left) + input.readAnalog(right),
                   -input.readAnalog(up) + input.readAnalog(down))
               * 100.f;
    };

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
        }

        /* LOGIC */
        setFillColor(dpadUp, Action::L_Up);
        setFillColor(dpadLeft, Action::L_Left);
        setFillColor(dpadDown, Action::L_Down);
        setFillColor(dpadRight, Action::L_Right);

        setFillColor(back, Action::Back);
        setFillColor(start, Action::Start);

        setFillColor(lbumper, Action::LBumper);
        setFillColor(rbumper, Action::RBumper);
        ltrigger.setSize({ 20.f, input.readAnalog(Action::LTrigger) * 100.f });
        rtrigger.setSize({ 20.f, input.readAnalog(Action::RTrigger) * 100.f });

        setFillColor(a, Action::A);
        setFillColor(b, Action::B);
        setFillColor(x, Action::X);
        setFillColor(y, Action::Y);

        setFillColor(lStickHat, Action::LStick);
        setFillColor(rStickHat, Action::RStick);

        auto lDirection = getDirection(
            Action::L_Up, Action::L_Down, Action::L_Left, Action::L_Right);
        auto rDirection = getDirection(
            Action::R_Up, Action::R_Down, Action::R_Left, Action::R_Right);

        lStickHat.setPosition(lStickOutline.getPosition() + lDirection);
        rStickHat.setPosition(rStickOutline.getPosition() + rDirection);

        /* DRAW */
        window.clear();

        window.draw(dpadUp);
        window.draw(dpadLeft);
        window.draw(dpadDown);
        window.draw(dpadRight);

        window.draw(back);
        window.draw(start);

        window.draw(lbumper);
        window.draw(rbumper);
        window.draw(ltrigger);
        window.draw(rtrigger);

        window.draw(a);
        window.draw(b);
        window.draw(x);
        window.draw(y);

        window.draw(lStickOutline);
        window.draw(rStickOutline);

        window.draw(lStickHat);
        window.draw(rStickHat);

        window.display();
    }

    return 0;
}