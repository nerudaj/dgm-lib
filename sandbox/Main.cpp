#include <DGM/dgm.hpp>

template<unsigned ParticleCount>
class AgentParticleSystemQuadratic : public dgm::ps::ParticleSystemInterface<ParticleCount>
{
public:
	using super = dgm::ps::ParticleSystemInterface<ParticleCount>;

public:
	AgentParticleSystemQuadratic() : super()
	{
		for (unsigned i = 0; i < super::particles.getCapacity(); i++)
		{
			auto&& particle = this->particles.growUnchecked();

			auto&& x = (i % 80) * 16;
			auto&& y = (i / 80) * 16;
			auto&& size = static_cast<float>(i % 5 + 1);
			auto&& position = sf::Vector2f(x + 8.f, y + 8.f);

			particle->spawn(
				position, sf::Vector2f(1.f, 1.f) * size);
			particle->setForward(
				dgm::Math::rotateVector(sf::Vector2f { 1.f, 0.f }, static_cast<float>(rand() % 360))
				* (6 - size) * 10.f);

			collisionBoxes.growUnchecked() = dgm::Circle(position, size / 2.f);
		}
	}

public:
	virtual void update(const dgm::Time& time) override
	{
		updateQuadratic(time);
	}

	void updateQuadratic(const dgm::Time& time)
	{
		for (unsigned i = 0; i < this->particles.getCapacity(); i++)
		{
			auto&& particle = this->particles[i];
			auto&& body = collisionBoxes[i];
			auto forward = particle->getForward();
			auto&& tickForward = forward * time.getDeltaTime();

			// O(n^2) check for collision
			bool secondCollisionFound = false;
			for (unsigned j = 0; j < collisionBoxes.getCapacity(); j++)
			{
				if (i == j) continue;

				dgm::Circle bodyCpy = body;
				bodyCpy.move(tickForward);

				if (dgm::Collision::basic(collisionBoxes[j], bodyCpy))
				{
					if (!secondCollisionFound)
					{
						tickForward *= -1.f;
						secondCollisionFound = true;
						forward *= -1.f;
					}
					else
					{
						tickForward = { 0.f, 0.f };
						break;
					}
				}
			}

			if ((body.getPosition().x + tickForward.x) < 0.f || (body.getPosition().x + tickForward.x) > 1280.f)
			{
				forward.x *= -1.f;
				tickForward.x *= -1.f;
			}

			if ((body.getPosition().y + tickForward.y) < 0.f || (body.getPosition().y + tickForward.y) > 720.f)
			{
				forward.y *= -1.f;
				tickForward.y *= -1.f;
			}

			particle->setForward(forward);
			particle->moveForwardBy(tickForward);
			body.move(tickForward);
		}
	}

private:
	dgm::StaticBuffer<dgm::Circle, ParticleCount> collisionBoxes;
};

template<unsigned ParticleCount>
class AgentParticleSystemLogarithmic : public dgm::ps::ParticleSystemInterface<ParticleCount>
{
public:
	using super = dgm::ps::ParticleSystemInterface<ParticleCount>;

public:
	AgentParticleSystemLogarithmic() : super(), collisionBoxes(sf::FloatRect(0.f, 0.f, 1280.f, 1280.f))
	{
		for (unsigned i = 0; i < super::particles.getCapacity(); i++)
		{
			auto&& particle = this->particles.growUnchecked();

			auto&& x = (i % 80) * 16;
			auto&& y = (i / 80) * 16;
			auto&& size = static_cast<float>(i % 5 + 1);
			auto&& position = sf::Vector2f(x + 8.f, y + 8.f);

			particle->spawn(
				position, sf::Vector2f(1.f, 1.f) * size);
			particle->setForward(
				dgm::Math::rotateVector(sf::Vector2f { 1.f, 0.f }, static_cast<float>(rand() % 360))
				* (6 - size) * 10.f);

			auto circle = dgm::Circle(position, size / 2.f);
			collisionBoxes.insert(circle, circle);
		}
	}

public:
	virtual void update(const dgm::Time& time) override
	{
		updateQuadratic(time);
	}

	void updateQuadratic(const dgm::Time& time)
	{
		for (unsigned i = 0; i < this->particles.getCapacity(); i++)
		{
			auto&& particle = this->particles[i];
			auto&& body = collisionBoxes[i];
			auto forward = particle->getForward();
			auto&& tickForward = forward * time.getDeltaTime();

			collisionBoxes.removeFromLookup(i, body);

			dgm::Circle bodyCpy = body;
			bodyCpy.move(tickForward);
			bool secondCollisionFound = false;
			auto&& candidates = collisionBoxes.getOverlapCandidates(bodyCpy);
			for (auto&& candidateId : candidates)
			{
				if (dgm::Collision::basic(bodyCpy, collisionBoxes[candidateId]))
				{
					if (!secondCollisionFound)
					{
						tickForward *= -1.f;
						secondCollisionFound = true;
						forward *= -1.f;
					}
					else
					{
						tickForward = { 0.f, 0.f };
						break;
					}
				}
			}

			if ((body.getPosition().x + tickForward.x) < 0.f || (body.getPosition().x + tickForward.x) > 1280.f)
			{
				forward.x *= -1.f;
				tickForward.x *= -1.f;
			}

			if ((body.getPosition().y + tickForward.y) < 0.f || (body.getPosition().y + tickForward.y) > 720.f)
			{
				forward.y *= -1.f;
				tickForward.y *= -1.f;
			}

			particle->setForward(forward);
			particle->moveForwardBy(tickForward);
			body.move(tickForward);

			collisionBoxes.addToLookup(i, body);
		}
	}

private:
	dgm::SpatialBuffer<dgm::Circle, 640> collisionBoxes;
};

int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280, 720 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	auto&& agentsViz = AgentParticleSystemLogarithmic<10000>();
	//auto&& agentsViz = AgentParticleSystemQuadratic<2000>();

	sf::Font font;
	std::cout << std::filesystem::current_path().string() << std::endl;
	font.loadFromFile("../../sandbox/segoe_ui.ttf");
	sf::Text text;
	text.setFont(font);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) std::ignore = window.close();
		}

		// Logic
		time.reset();

		agentsViz.update(time);
		text.setString(std::to_string(static_cast<unsigned>(1.f / time.getDeltaTime())));

		// Draw
		window.beginDraw();

		agentsViz.draw(window);
		window.draw(text);

		window.endDraw();
	}

	return 0;
}
