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

#include "Helpers.hpp"
#include <DGM/dgm.hpp>

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Math", false);
    auto&& circle = dgm::Circle({ 1000.f, 500.f }, 140.f);

    // Line is defined as direction and position
    auto&& center = sf::Vector2f(window.getSize() / 2u);
    auto&& point1 = sf::Vector2f(100.f, 0.f);
    auto&& point2 = sf::Vector2f(300.f, 720.f);
    auto&& line = dgm::Math::Line(point2 - point1, point1);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
        }

        auto&& mousePos =
            sf::Vector2f(sf::Mouse::getPosition(window.getSfmlWindowContext()));
        auto&& mouseDir = dgm::Math::toUnit(mousePos - center);
        auto&& mouseLine = dgm::Math::Line(mouseDir, center);

        // This is the bulk of the example - compute intersections between lines
        // and objects
        auto&& lineIntersection = dgm::Math::getIntersection(mouseLine, line);
        auto&& circleIntersections =
            dgm::Math::getIntersection(mouseLine, circle);
        auto&& closestLinePoint =
            dgm::Math::getClosestPointOnLine(line, mousePos);

        window.clear();

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

        window.display();
    }

    return 0;
}
