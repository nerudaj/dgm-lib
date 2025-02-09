#include "Actor.hpp"
#include "../shared/Level.hpp"

void Actor::update(const dgm::Time& dt, const Scene& scene)
{
	sf::Vector2f forward = { 0.f, 0.f };

	if (input.goUp())
		forward.y = -SPEED;
	else if (input.goDown())
		forward.y = SPEED;

	if (input.goLeft())
		forward.x = -SPEED;
	else if (input.goRight())
		forward.x = SPEED;

	const auto forwardSize = dgm::Math::getSize(forward);
	if (forwardSize > 0.f)
		lookDir = forward / forwardSize;

	auto momentFwd = forward * dt.getDeltaTime();
	dgm::Collision::advanced(scene.level.getMesh(), body, momentFwd);
	body.move(momentFwd);
}