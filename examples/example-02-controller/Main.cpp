/*
An example showing how to bind keyboard, mouse and xbox to
abstract inputs.
*/

#include <DGM/dgm.hpp>

// Each physical input needs to be mapped to
// some abstract action represented as a value
// in this enum.
enum Action
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
    dgm::Controller input;

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

    // Bind controller
    input.bindInput(Action::L_Up, dgm::Xbox::Button::DPadUp);
    input.bindInput(Action::L_Left, dgm::Xbox::Button::DPadLeft);
    input.bindInput(Action::L_Down, dgm::Xbox::Button::DPadDown);
    input.bindInput(Action::L_Right, dgm::Xbox::Button::DPadRight);
    input.bindInput(Action::L_Up, dgm::Xbox::Axis::LStickYneg);
    input.bindInput(Action::L_Left, dgm::Xbox::Axis::LStickXneg);
    input.bindInput(Action::L_Down, dgm::Xbox::Axis::LStickYpos);
    input.bindInput(Action::L_Right, dgm::Xbox::Axis::LStickXpos);
    input.bindInput(Action::R_Up, dgm::Xbox::Axis::RStickYneg);
    input.bindInput(Action::R_Left, dgm::Xbox::Axis::RStickXneg);
    input.bindInput(Action::R_Down, dgm::Xbox::Axis::RStickYpos);
    input.bindInput(Action::R_Right, dgm::Xbox::Axis::RStickXpos);
    input.bindInput(Action::A, dgm::Xbox::Button::A);
    input.bindInput(Action::B, dgm::Xbox::Button::B);
    input.bindInput(Action::X, dgm::Xbox::Button::X);
    input.bindInput(Action::Y, dgm::Xbox::Button::Y);
    input.bindInput(Action::Back, dgm::Xbox::Button::Back);
    input.bindInput(Action::Start, dgm::Xbox::Button::Start);
    input.bindInput(Action::LBumper, dgm::Xbox::Button::LBumper);
    input.bindInput(Action::RBumper, dgm::Xbox::Button::RBumper);
    input.bindInput(Action::LTrigger, dgm::Xbox::Axis::LTrigger);
    input.bindInput(Action::RTrigger, dgm::Xbox::Axis::RTrigger);
    input.bindInput(Action::LStick, dgm::Xbox::Button::LStick);
    input.bindInput(Action::RStick, dgm::Xbox::Button::RStick);

    // Bind mouse
    input.bindInput(Action::LTrigger, sf::Mouse::Button::Left);
    input.bindInput(Action::RTrigger, sf::Mouse::Button::Right);

    // Decorations
    auto&& text = sf::Text(resmgr.get<sf::Font>("cruft.ttf"));

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
            input.isInputToggled(action) ? sf::Color::Green : sf::Color::Red);
    };

    auto getDirection =
        [&input](
            Action up, Action down, Action left, Action right) -> sf::Vector2f
    {
        return sf::Vector2f(
                   input.getInputValue(left) + input.getInputValue(right),
                   input.getInputValue(up) + input.getInputValue(down))
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
        input.update();

        setFillColor(dpadUp, Action::L_Up);
        setFillColor(dpadLeft, Action::L_Left);
        setFillColor(dpadDown, Action::L_Down);
        setFillColor(dpadRight, Action::L_Right);

        setFillColor(back, Action::Back);
        setFillColor(start, Action::Start);

        setFillColor(lbumper, Action::LBumper);
        setFillColor(rbumper, Action::RBumper);
        ltrigger.setSize(
            { 20.f, input.getInputValue(Action::LTrigger) * 100.f });
        rtrigger.setSize(
            { 20.f, input.getInputValue(Action::RTrigger) * 100.f });

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