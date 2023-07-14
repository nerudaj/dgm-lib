#include <DGM/dgm.hpp>


int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280, 720 }, "Sandbox", false);
	sf::Event event;
	dgm::Time time;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) std::ignore = window.close();
		}

		// Logic
		time.reset();

		// Draw
		window.beginDraw();

		window.endDraw();
	}

	return 0;
}
