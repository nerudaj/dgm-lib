#include <DGM\dgm.hpp>

using dgm::ps::Particle;

void Particle::changeFrame(const sf::IntRect &frame) {
	quad[0].texCoords = sf::Vector2f(float(frame.left), float(frame.top));
	quad[1].texCoords = sf::Vector2f(float(frame.left + frame.width), float(frame.top));
	quad[2].texCoords = sf::Vector2f(float(frame.left + frame.width), float(frame.top + frame.height));
	quad[3].texCoords = sf::Vector2f(float(frame.left), float(frame.top + frame.height));
}

void dgm::ps::Particle::setColor(const sf::Color& color) {
	for (int i = 0; i < 4; i++) {
		quad[i].color = color;
	}
}

void Particle::spawn(const sf::Vector2f &position) {
	sf::Vector2f offsets[] = {
		{0.f, 0.f},
		{size.x, 0.f},
		{size.x, size.y},
		{0.f, size.y}
	};
	
	for (int i = 0; i < 4; i++) {
		quad[i].position = position + offsets[i];
	}
}

void Particle::move(const sf::Vector2f &forward) {
	for (int i = 0; i < 4; i++) {
		quad[i].position += forward;
	}
}

void Particle::destroy() {
	lifespan = 0.f;
	for (int i = 0; i < 4; i++) {
		quad[i].position = {0.f, 0.f};
	}
}

void Particle::init(sf::Vertex *vertices) {
	quad = vertices;
}
