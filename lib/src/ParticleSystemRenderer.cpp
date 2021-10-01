#include <DGM\dgm.hpp>
#include "..\include\DGM\classes\ParticleSystemRenderer.hpp"

void dgm::ps::ParticleSystemRenderer::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	// apply the transform
	states.transform *= getTransform();

	// apply the tileset texture
	states.texture = texture;

	// draw the vertex array
	target.draw(vertices, states);
}

sf::Vertex * dgm::ps::ParticleSystemRenderer::getParticleVertices(const std::size_t index) {
	assert(index < vertices.getVertexCount() / 4);

	return &vertices[index * 4];
}

bool dgm::ps::ParticleSystemRenderer::init(const std::size_t particleCount) {
	try {
		vertices.clear();
		vertices.setPrimitiveType(sf::Quads);
		vertices.resize(particleCount * 4);
	}
	catch (...) {
		std::cerr << "ParticleSystemRenderer::init(...) - Could not allocate memory\n";
		return false;
	}

	return true;
}

void dgm::ps::ParticleSystemRenderer::setTexture(sf::Texture& texture) {
	ParticleSystemRenderer::texture = &texture;
}
