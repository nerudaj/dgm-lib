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
    shape.setOrigin(radius, radius);
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

void dgm::Rect::setPosition(const float x, const float y)
{
    position.x = x;
    position.y = y;
}

void dgm::Rect::setSize(const float width, const float height)
{
    size.x = width;
    size.y = height;
}

// ************
// *** MESH ***
// ************
void dgm::Mesh::setDataSize(const sf::Vector2u& size)
{
    data.clear();
    data.resize(size.x * size.y, 0);
    dataSize = size;
}

void dgm::Mesh::move(const float x, const float y)
{
    position.x += x;
    position.y += y;
}

void dgm::Mesh::move(const sf::Vector2f& forward)
{
    position.x += forward.x;
    position.y += forward.y;
}

dgm::Mesh::Mesh(
    const std::vector<int>& data,
    const sf::Vector2u& dataSize,
    const sf::Vector2u& voxelSize)
    : data(data), dataSize(dataSize), voxelSize(voxelSize)
{
    assert(
        data.size() == dataSize.x * dataSize.y
        && "Mesh data.size() must equal dataSize.x * dataSize.y");
}

// *******************
// *** VISION CONE ***
// *******************
void dgm::VisionCone::debugRender(dgm::Window& window, sf::Color color) const
{
    const sf::Vector2f unit = forward / dgm::Math::getSize(forward);
    const sf::Vector2f plane = sf::Vector2f(unit.y, -unit.x) * width / 2.f;

    sf::ConvexShape shape(3);
    shape.setPoint(0, position);
    shape.setPoint(1, forward + position - plane);
    shape.setPoint(2, forward + position + plane);
    shape.setFillColor(color);

    window.draw(shape);
}

void dgm::VisionCone::move(const float x, const float y)
{
    position.x += x;
    position.y += y;
}

void dgm::VisionCone::setRotation(const float angle)
{
    rotation = angle;
    forward = dgm::Math::polarToCartesian(
        angle, dgm::Math::getSize(forward));
}

void dgm::VisionCone::rotate(const float angle)
{
    rotation += angle;
    forward = dgm::Math::getRotated(forward, angle);
}

float dgm::VisionCone::getLength() const
{
    return dgm::Math::getSize(forward);
}

dgm::VisionCone::VisionCone(const float length, const float width)
{
    position = { 0.f, 0.f };
    forward = { length, 0.f };
    VisionCone::width = width;
}
