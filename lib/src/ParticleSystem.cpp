#include <DGM/classes/ParticleSystem.hpp>
#include <DGM/classes/Particle.hpp>

using dgm::ps::ParticleSystemInterface;

/*float dgm::ps::ParticleSystemInterface::randomFloat(float min, float max) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX) / (max - min)) + min;
}*/

dgm::ps::Particle* dgm::ps::ParticleSystemInterface::createParticle(sf::Vertex* vertices, unsigned) const {
	return new dgm::ps::Particle(vertices);
}

// *** PARTICLE SYSTEM ***
void ParticleSystemInterface::init(const std::size_t particleCount) {
	try {
		renderer.init(particleCount);
		particles.resize(particleCount);

		for (unsigned i = 0; i < particles.capacity(); i++) {
			particles[i] = createParticle(renderer.getParticleVertices(i), i);
		}
	}
	catch (...) {
		std::cerr << "dgm::ParticleSystem: Memory allocation failure" << std::endl;
		throw;
	}
}

dgm::ps::ParticleSystemInterface::~ParticleSystemInterface() {
	for (unsigned i = 0; i < particles.capacity(); i++) {
		delete particles[i];
	}
}

