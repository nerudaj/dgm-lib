#include <DGM/dgm.hpp>


int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280, 720 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	auto&& circle = dgm::Circle(sf::Vector2f(1000.f, 500.f), 100.f);
	auto&& dot = dgm::Circle(sf::Vector2f(0.f, 0.f), 5.f);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) std::ignore = window.close();
		}

		// Logic
		time.reset();

		auto&& mousePos = sf::Vector2f(sf::Mouse::getPosition(window.getWindowContext()));

		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y))
		};

		auto&& intersects = dgm::Math::getIntersection(
			dgm::Math::Line(mousePos, sf::Vector2f(0.f, 0.f)), circle);

		// Draw
		window.beginDraw();

		window.getWindowContext().draw(line, 2, sf::Lines);
		circle.debugRender(window);

		if (intersects)
		{
			auto&& pair = intersects.value();
			dot.setPosition(pair.first);
			dot.debugRender(window, sf::Color::Red);
			dot.setPosition(pair.second);
			dot.debugRender(window, sf::Color::Red);
		}

		window.endDraw();
	}

	return 0;
}
