/**
 *  \file Objects.cpp
 *  \author doomista
 */

#include <DGM\dgm.hpp>

// **************
// *** CIRCLE ***
// **************
void dgm::Circle::debugRender(dgm::Window& window, sf::Color color) const {
	sf::CircleShape shape;
	shape.setRadius(radius);
	shape.setOrigin(radius, radius);
	shape.setPosition(position);
	shape.setFillColor(color);
	window.draw(shape);
}

const sf::Vector2f &dgm::Circle::getPosition() const { return position; }

const float dgm::Circle::getRadius() const { return radius; }
		
void dgm::Circle::setPosition(const float x, const float y) {
	position.x = x;
	position.y = y;
}

void dgm::Circle::setPosition(const sf::Vector2f &position) { Circle::position = position; }

void dgm::Circle::setRadius(const float radius) { Circle::radius = radius; }

void dgm::Circle::move(const float x, const float y) {
	position.x += x;
	position.y += y;
}

void dgm::Circle::move(const sf::Vector2f &forward) {
	position.x += forward.x;
	position.y += forward.y;
}

dgm::Circle::Circle() {
	setPosition(0.f, 0.f);
	radius = 0.f;
}

dgm::Circle::Circle(const float x, const float y) { setPosition(x, y); }
dgm::Circle::Circle(const sf::Vector2f &position) { setPosition(position); }
dgm::Circle::~Circle() {}

// ************
// *** RECT ***
// ************
void dgm::Rect::debugRender(dgm::Window& window, sf::Color color) const {
	sf::RectangleShape shape;
	shape.setSize(size);
	shape.setPosition(position);
	shape.setFillColor(color);
	window.draw(shape);
}

const sf::Vector2f &dgm::Rect::getPosition() const { return position; }

const sf::Vector2f &dgm::Rect::getSize() const { return size; }

const sf::Vector2f& dgm::Rect::getCenter() const {
	return getPosition() + getSize() / 2.f;
}

void dgm::Rect::setPosition(const float x, const float y) {
	position.x = x;
	position.y = y;
}

void dgm::Rect::setPosition(const sf::Vector2f &position) { Rect::position = position; }

void dgm::Rect::move(const float x, const float y) {
	position.x += x;
	position.y += y;
}

void dgm::Rect::move(const sf::Vector2f &forward) {
	position.x += forward.x;
	position.y += forward.y;
}

void dgm::Rect::setSize(const float width, const float height) {
	size.x = width;
	size.y = height;
}

void dgm::Rect::setSize(const sf::Vector2f &size) { Rect::size = size; }

dgm::Rect::Rect() {
	setPosition(0.f, 0.f);
	setSize(0.f, 0.f);
}

dgm::Rect::Rect(const float x, const float y, const float width, const float height) {
	setPosition(x, y);
	setSize(width, height);
}

dgm::Rect::Rect(const sf::Vector2f &position, const sf::Vector2f &size) {
	setPosition(position);
	setSize(size);
}

dgm::Rect::~Rect() {
	
}

// ************
// *** MESH ***
// ************
void dgm::Mesh::setDataSize(const sf::Vector2u &size) {
	data.clear();
	data.resize(size.x * size.y, 0);
	dataSize = size;
}

void dgm::Mesh::move(const float x, const float y) {
	position.x += x;
	position.y += y;
}

void dgm::Mesh::move(const sf::Vector2f &forward) {
	position.x += forward.x;
	position.y += forward.y;
}

dgm::Mesh::Mesh() {
	position = sf::Vector2f(0.f, 0.f);
	dataSize = sf::Vector2u(0, 0);
	voxelSize= sf::Vector2u(0, 0);
}

dgm::Mesh::Mesh(const LevelD::Mesh& mesh, unsigned layerIndex) {
	voxelSize = sf::Vector2u(mesh.tileWidth, mesh.tileHeight);
	dataSize = sf::Vector2u(mesh.layerWidth, mesh.layerHeight);
	data = std::vector<int>(mesh.layers[layerIndex].blocks.begin(), mesh.layers[layerIndex].blocks.end());
}
