#include <DGM/dgm.hpp>

int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280,720 }, "Test", false);
	sf::Event event;
	
	dgm::Circle circle(640.f, 360.f, 20.f);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) std::ignore = window.close();
		}

		window.beginDraw();

		circle.debugRender(window, sf::Color::Red);

		window.endDraw();
	}

	return 0;
}
