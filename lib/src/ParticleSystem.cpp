#include <DGM\dgm.hpp>

using dgm::ps::ParticleSystemInterface;

float dgm::ps::ParticleSystemInterface::randomFloat(float min, float max) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX) / (max - min)) + min;
}

dgm::ps::Particle* dgm::ps::ParticleSystemInterface::createParticle(unsigned) const {
	return new dgm::ps::Particle;
}

// *** PARTICLE SYSTEM ***
bool ParticleSystemInterface::init(const std::size_t particleCount) {
	if (not renderer.init(particleCount)) return false;
	
	try {
		particles.resize(particleCount);

		for (unsigned i = 0; i < particles.capacity(); i++) {
			particles[i] = createParticle(i);
			particles[i]->init(renderer.getParticleVertices(i));
		}
	}
	catch(...) {
		return false;
	}
	
	return true;
}

dgm::ps::ParticleSystemInterface::ParticleSystemInterface() {
}

dgm::ps::ParticleSystemInterface::~ParticleSystemInterface() {
	for (unsigned i = 0; i < particles.capacity(); i++) {
		delete particles[i];
	}
}

