#include <DGM/dgm.hpp>

class RotatingParticleSystem : public dgm::ps::ParticleSystemInterface {
	float rotationSpeed = 75.f; // degrees per second

public:
	virtual void update(const dgm::Time& time) override {
		particles[0]->rotateBy(rotationSpeed * time.getDeltaTime());
	}

	virtual void init(const std::size_t particleCount) override {
		ParticleSystemInterface::init(particleCount);

		particles.expand();
		auto& p = particles.last();
		p->spawn(sf::Vector2f(1920.f, 1080.f) / 2.f, sf::Vector2f(256.f, 256.f), sf::Time::Zero);
		p->setColor(sf::Color::Red);
	}
};

int main(int argc, char *argv[]) {
	dgm::Window window({ 1920, 1080 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	RotatingParticleSystem rps;
	rps.init(1);

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		// Logic
		time.reset();
		rps.update(time);

		// Draw
		window.beginDraw();

		rps.draw(window);

		window.endDraw();
	}

	return 0;
}
