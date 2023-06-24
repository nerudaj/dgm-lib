#include <DGM/dgm.hpp>

int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280,720 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	auto&& cam = dgm::Camera{ { 0.f, 0.f, 1.f, 1.f }, sf::Vector2f{ window.getSize()} };
	cam.setPosition({ 0.f, 0.f });
	dgm::VisionCone cone(100.f, 50.f);
	dgm::Circle circle(0.f, 0.f, 20.f);
	cone.setPosition(100.f, 100.f);
	circle.setPosition(215.f, 100.f);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) std::ignore = window.close();
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Z)
				{
					cam.setZoom(0.5f);
				}
				else if (event.key.code == sf::Keyboard::Y)
				{
					cam.setZoom(1.f);
				}
			}
		}

		// Logic
		time.reset();

		// Draw
		window.getWindowContext().setView(cam.getCurrentView());
		window.beginDraw();

		circle.debugRender(window, sf::Color::Red);
		cone.debugRender(window, sf::Color::Yellow);

		window.endDraw();
	}

	return 0;
}
