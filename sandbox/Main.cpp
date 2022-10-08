#include <DGM/dgm.hpp>

int main(int, char* [])
{
	dgm::Window window({ 1920, 1080 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	dgm::VisionCone cone(100.f, 50.f);
	dgm::Circle circle(0.f, 0.f, 20.f);
	cone.setPosition(100.f, 100.f);
	circle.setPosition(215.f, 100.f);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		// Logic
		time.reset();

		// Draw
		window.beginDraw();

		circle.debugRender(window, sf::Color::Red);
		cone.debugRender(window, sf::Color::Yellow);

		window.endDraw();
	}

	return 0;
}
