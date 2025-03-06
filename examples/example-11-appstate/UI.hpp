#pragma once

#include <DGM/dgm.hpp>

struct Button
{
    std::string label;
    dgm::Rect box;
};

enum class Alignment
{
    Left,
    Center,
};

sf::RectangleShape createBox(
    const sf::Vector2f& position, const sf::Vector2f& size, sf::Color color)

{
    sf::RectangleShape shape(size);
    shape.setFillColor(color);
    shape.setPosition(position);
    return shape;
}

std::vector<Button> computeLayout(
    const std::initializer_list<std::string>& labels,
    Alignment alignment,
    const sf::Vector2f& windowSize)
{
    const float BTN_OFFSET = 15.f;

    auto&& btnSize =
        sf::Vector2f { windowSize.x / 2.f - 2 * BTN_OFFSET,
                       (windowSize.y - (labels.size() + 1) * BTN_OFFSET)
                           / labels.size() };
    std::vector<Button> buttons;

    for (auto&& [idx, label] : std::views::enumerate(labels))
    {
        auto&& btnPos =
            alignment == Alignment::Left
                ? sf::Vector2f { BTN_OFFSET,
                                 idx * (btnSize.y + BTN_OFFSET) + BTN_OFFSET }
                : sf::Vector2f { windowSize.x / 4.f + BTN_OFFSET,
                                 idx * (btnSize.y + BTN_OFFSET) + BTN_OFFSET };

        buttons.push_back(Button {
            .label = label,
            .box = dgm::Rect(btnPos, btnSize),
        });
    }

    return buttons;
}

void renderButtons(
    const std::vector<Button>& buttons,
    sf::Text& text,
    dgm::Window& window,
    sf::Color backgroundColor = sf::Color::Transparent)
{
    auto&& size = window.getSize();
    auto&& background = createBox(
        sf::Vector2f { 0.f, 0.f }, sf::Vector2f { size }, backgroundColor);
    window.draw(background);

    const float BTN_OFFSET = 15.f;

    for (auto&& [idx, button] : std::views::enumerate(buttons))
    {
        button.box.debugRender(window, sf::Color(96, 96, 96));

        text.setString(button.label);
        text.setPosition(
            button.box.getCenter() - text.getGlobalBounds().size / 2.f);
        window.draw(text);
    }
}
