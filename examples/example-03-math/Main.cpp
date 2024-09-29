/*
* An example how to use various Math helpers for
* finding intersections between lines and circles.
*
* Yellow line aims in the direction to the mouse and always
* goes through the window center.
*
* When it intersects a white line, that intersection is
* highlighted with a red dot.
*
* Same as with intersections with the circle.
*
* Blue line is the shortest distance from the mouse to the
* white line.
*/

#include <DGM/dgm.hpp>

void drawLine(
	dgm::Window& window,
	const sf::Vector2f& a,
	const sf::Vector2f& b,
	dgm::UniversalReference<sf::Color> auto&& color)
{
	sf::Vertex line[] =
	{
		sf::Vertex(a),
		sf::Vertex(b)
	};

	line[0].color = color;
	line[1].color = color;

	window.getWindowContext().draw(line, 2, sf::Lines);
}

void drawDot(dgm::Window& window, const sf::Vector2f& position, const sf::Color& color = sf::Color::Red)
{
	auto&& dot = dgm::Circle(position, 3.f);
	dot.debugRender(window, color);
}

int main()
{
	auto&& window = dgm::Window({ 1280, 720 }, "Example: App", false);
	sf::Event event;

	auto&& circle = dgm::Circle(1000.f, 500.f, 140.f);

	// Line is defined as direction and position
	auto&& center = sf::Vector2f(window.getSize() / 2u);
	auto&& point1 = sf::Vector2f(100.f, 0.f);
	auto&& point2 = sf::Vector2f(300.f, 720.f);
	auto&& line = dgm::Math::Line(point2 - point1, point1);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				std::ignore = window.close();
		}

		auto&& mousePos = sf::Vector2f(sf::Mouse::getPosition(window.getWindowContext()));
		auto&& mouseDir = dgm::Math::toUnit(mousePos - center);
		auto&& mouseLine = dgm::Math::Line(mouseDir, center);

		auto&& lineIntersection = dgm::Math::getIntersection(mouseLine, line);
		auto&& circleIntersections = dgm::Math::getIntersection(mouseLine, circle);
		auto&& closestLinePoint = dgm::Math::getClosestPointOnLine(line, mousePos);

		window.beginDraw();

		circle.debugRender(window, sf::Color::White);
		drawLine(window, point1, point2, sf::Color::White);
		drawLine(
			window,
			center - mouseDir * 1080.f,
			center + mouseDir * 1080.f,
			sf::Color::Yellow);

		if (lineIntersection)
		{
			drawDot(window, *lineIntersection);
		}

		if (circleIntersections)
		{
			drawDot(window, circleIntersections->first);
			drawDot(window, circleIntersections->second);
		}

		drawDot(window, closestLinePoint, sf::Color::Green);
		drawLine(window, mousePos, closestLinePoint, sf::Color::Blue);

		window.endDraw();
	}

	return 0;
}
