#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Window.hpp>
#include <DGM/classes/Math.hpp>
#include <DGM/classes/Conversion.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

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

dgm::Mesh::Mesh()
{
	position = sf::Vector2f(0.f, 0.f);
	dataSize = sf::Vector2u(0, 0);
	voxelSize = sf::Vector2u(0, 0);
}

dgm::Mesh::Mesh(const LevelD::Mesh& mesh, unsigned layerIndex)
{
	voxelSize = sf::Vector2u(mesh.tileWidth, mesh.tileHeight);
	dataSize = sf::Vector2u(mesh.layerWidth, mesh.layerHeight);
	data = std::vector<int>(mesh.layers[layerIndex].blocks.begin(), mesh.layers[layerIndex].blocks.end());
}

// *******************
// *** VISION CONE ***
// *******************
void dgm::VisionCone::debugRender(dgm::Window& window, sf::Color color) const
{
	const sf::Vector2f unit = forward / dgm::Math::vectorSize(forward);
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
	forward = dgm::Conversion::polarToCartesian(angle, dgm::Math::vectorSize(forward));
}

void dgm::VisionCone::rotate(const float angle)
{
	rotation += angle;
	forward = dgm::Math::rotateVector(forward, angle);
}

float dgm::VisionCone::getLength() const
{
	return dgm::Math::vectorSize(forward);
}

dgm::VisionCone::VisionCone(const float length, const float width)
{
	position = { 0.f, 0.f };
	forward = { length, 0.f };
	VisionCone::width = width;
}
