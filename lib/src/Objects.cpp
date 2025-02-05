#include <DGM/classes/Math.hpp>
#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Window.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

// **************
// *** CIRCLE ***
// **************
void dgm::Circle::debugRender(dgm::Window& window, sf::Color color) const
{
    sf::CircleShape shape;
    shape.setRadius(radius);
    shape.setOrigin({ radius, radius });
    shape.setPosition(position);
    shape.setFillColor(color);
    window.draw(shape);
}

// ************
// *** RECT ***
// ************
void dgm::Rect::debugRender(dgm::Window& window, sf::Color color) const
{
    sf::RectangleShape shape;
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(color);
    window.draw(shape);
}

// *******************
// *** VISION CONE ***
// *******************
void dgm::VisionCone::debugRender(dgm::Window& window, sf::Color color) const
{
    const sf::Vector2f unit = dgm::Math::toUnit(forward);
    const sf::Vector2f plane = sf::Vector2f(unit.y, -unit.x) * width / 2.f;

    sf::ConvexShape shape(3);
    shape.setPoint(0, position);
    shape.setPoint(1, forward + position - plane);
    shape.setPoint(2, forward + position + plane);
    shape.setFillColor(color);

    window.draw(shape);
}

void dgm::VisionCone::setRotation(const sf::Angle angle) noexcept
{
    rotation = angle;
    forward = sf::Vector2f(forward.length(), angle);
}

void dgm::VisionCone::rotate(const sf::Angle angle) noexcept
{
    rotation += angle;
    forward = forward.rotatedBy(angle);
}

dgm::VisionCone::VisionCone(const float length, const float width)
{
    position = { 0.f, 0.f };
    forward = { length, 0.f };
    VisionCone::width = width;
}
