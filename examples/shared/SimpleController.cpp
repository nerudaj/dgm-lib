#include "SimpleController.hpp"

SimpleController::SimpleController()
{
    input.bindInput(Action::Up, sf::Keyboard::Key::W);
    input.bindInput(Action::Down, sf::Keyboard::Key::S);
    input.bindInput(Action::Left, sf::Keyboard::Key::A);
    input.bindInput(Action::Right, sf::Keyboard::Key::D);
}

sf::Vector2f SimpleController::getForward() const
{
    return dgm::Math::toUnit(sf::Vector2f {
        -input.getInputValue(Action::Left) + input.getInputValue(Action::Right),
        -input.getInputValue(Action::Up) + input.getInputValue(Action::Down),
    });
}
