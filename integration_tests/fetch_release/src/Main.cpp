#include <DGM/dgm.hpp>

int main(int, char*[])
{
    auto&& window = dgm::Window({ 1280, 720 }, "Test", false);

    dgm::Circle circle(640.f, 360.f, 20.f);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) std::ignore = window.close();
        }

        window.clear();

        circle.debugRender(window, sf::Color::Red);

        window.display();
    }

    return 0;
}
