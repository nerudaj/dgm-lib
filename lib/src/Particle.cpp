#include <DGM/classes/Particle.hpp>

using dgm::ps::Particle;

void Particle::setAnimationFrame(const sf::IntRect &frame) noexcept {
	quad[0].texCoords = sf::Vector2f(float(frame.left), float(frame.top));
	quad[1].texCoords = sf::Vector2f(float(frame.left + frame.width), float(frame.top));
	quad[2].texCoords = sf::Vector2f(float(frame.left + frame.width), float(frame.top + frame.height));
	quad[3].texCoords = sf::Vector2f(float(frame.left), float(frame.top + frame.height));
}

void Particle::spawn(const sf::Vector2f& newPosition, const sf::Vector2f& newSize, const sf::Time& newLifespan) {
	lifespan = newLifespan.asSeconds();
	size = newSize;

	sf::Vector2f offsets[] = {
		{0.f, 0.f},
		{size.x, 0.f},
		{size.x, size.y},
		{0.f, size.y}
	};
	
	for (int i = 0; i < 4; i++) {
		quad[i].position = newPosition + offsets[i];
	}
}
