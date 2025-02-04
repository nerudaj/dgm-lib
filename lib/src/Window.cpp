#include <DGM/classes/Window.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

void dgm::Window::open(
    const sf::Vector2u& resolution,
    const std::string& windowTitle,
    const bool forceFullscreen)
{
    title = windowTitle;
    style = sf::Style::Titlebar | sf::Style::Close;
    fullscreen = forceFullscreen;

    window.create(
        sf::VideoMode { sf::Vector2u(resolution.x, resolution.y), 32 },
        windowTitle,
        style,
        forceFullscreen ? sf::State::Fullscreen : sf::State::Windowed);
}

dgm::WindowSettings dgm::Window::close()
{
    const WindowSettings settings = {
        .resolution = getSize(),
        .title = title,
        .useFullscreen = fullscreen,
    };

    window.close();

    return settings;
}

void dgm::Window::toggleFullscreen()
{
    auto size = getSize();
    std::ignore = close();
    fullscreen = !fullscreen;
    open(size, title, fullscreen);
}

void dgm::Window::changeResolution(const sf::Vector2u& newResolution)
{
    std::ignore = close();
    open(newResolution, title, fullscreen);
}

sf::Image dgm::Window::getScreenshot() const
{
    sf::Vector2u windowSize = window.getSize();
    sf::Texture texture = sf::Texture(sf::Vector2u(windowSize.x, windowSize.y));
    texture.update(window);
    return texture.copyToImage();
}
