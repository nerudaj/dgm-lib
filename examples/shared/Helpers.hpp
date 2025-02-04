#pragma once

#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Window.hpp>

static inline void drawLine(
    dgm::Window& window,
    const sf::Vector2f& a,
    const sf::Vector2f& b,
    const sf::Color& color)
{
    sf::Vertex line[] = { sf::Vertex(a, color), sf::Vertex(b, color) };
    window.getWindowContext().draw(line, 2, sf::PrimitiveType::Lines);
}

static inline void drawDot(
    dgm::Window& window,
    const sf::Vector2f& position,
    const sf::Color& color = sf::Color::Red)
{
    auto&& dot = dgm::Circle(position, 3.f);
    dot.debugRender(window, color);
}
