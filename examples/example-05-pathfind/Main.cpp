/*
* example-pathfind - Click anywhere in the map and yellow circle will compute optimal path
* and move to target spot. Click on invalid spot (outside of the map or a wall) and nothing
* will happen.
*/

#include <DGM/dgm.hpp>
#include "../shared/Level.hpp"
#include "../shared/DemoData.hpp"
#include "../ResourceDir.hpp"

class Actor
{
private:
	const float SPEED = 256.f;
	const float RADIUS = 16.f;

	dgm::Circle body = dgm::Circle(100.f, 100.f, RADIUS);
	dgm::WorldNavMesh navMesh;
	dgm::Path<dgm::WorldNavpoint> path;

	sf::Vector2f oldPosition = { 0.f, 0.f };
	sf::Vector2f forward = { 0.f, 0.f };
	float transitionTimer = 0.f;
	float transitionDuration = 0.f;

	void beginTransitionToNextPoint()
	{
		if (path.isTraversed()) return;

		oldPosition = body.getPosition();
		forward = path.getCurrentPoint().coord - body.getPosition();
		transitionTimer = 0.f;
		transitionDuration = dgm::Math::getSize(forward) / SPEED;
	}

public:
	enum
	{
		Up, Left, Right, Down
	};

	void draw(dgm::Window& window)
	{
		body.debugRender(window);
	}

	void update(const dgm::Time& time)
	{
		if (path.isTraversed()) return;

		transitionTimer += time.getDeltaTime();
		const auto transition = std::clamp(transitionTimer / transitionDuration, 0.f, 1.f);
		body.setPosition(oldPosition + forward * transition);

		if (transition >= 1.f)
		{
			path.advance();
			beginTransitionToNextPoint();
		}
	}

	void setWaypoint(const sf::Vector2f& point)
	{
		try
		{
			std::cout << dgm::Utility::to_string(body.getPosition()) << " -> " << dgm::Utility::to_string(point) << std::endl;
			path = std::move(navMesh.getPath(body.getPosition(), point));
		}
		catch (...)
		{
			path = dgm::Path<dgm::WorldNavpoint>();
		}

		beginTransitionToNextPoint();
	}

	Actor(const dgm::Mesh& mesh) : navMesh(mesh) {}
};

int main()
{
	dgm::Window window({ 1280, 980 }, "Example: Pathfind", false);
	dgm::Time time;

	dgm::ResourceManager resmgr;
	resmgr.loadResourcesFromDirectory<sf::Texture>(
		RESOURCE_DIR,
		[](const std::filesystem::path& path, sf::Texture& texture)
		{
			texture.loadFromFile(path.string());
		},
		{ ".png" });

	Level level(resmgr.get<sf::Texture>("tileset.png").value().get());
	level.loadFromLvd(DemoData::createDemoLevel());

	Actor actor(level.getMesh());

	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				auto mousePos = sf::Mouse::getPosition(window.getWindowContext());
				actor.setWaypoint(sf::Vector2f(mousePos));
			}
		}

		/* LOGIC */
		time.reset();

		actor.update(time);

		/* DRAW */
		window.beginDraw();

		level.draw(window);
		actor.draw(window);

		window.endDraw();
	}

	return 0;
}
