#include <DGM/classes/Window.hpp>

#include <SFML/Graphics/Texture.hpp>

void dgm::Window::open(const cfg::Ini & config) {
	if (config.hasSection("Window")) {
		auto section = config["Window"];
		open(sf::Vector2u(section["width"].asInt(), section["height"].asInt()), section["title"].asString(), section["fullscreen"].asBool());
	}
	else {
		open({1280, 720}, "", false);
	}
}

void dgm::Window::open(const sf::Vector2u & resolution, const std::string & windowTitle, const bool forceFullscreen) {
	title = windowTitle;
	style = forceFullscreen ? sf::Style::Fullscreen : (sf::Style::Titlebar | sf::Style::Close);
	fullscreen = forceFullscreen;

	window.create({ resolution.x, resolution.y, 32 }, windowTitle, style);
}

void dgm::Window::close(cfg::Ini & config) {
	auto size = getSize();

	config["Window"]["width"] = int(size.x);
	config["Window"]["height"] = int(size.y);
	config["Window"]["title"] = title;
	config["Window"]["fullscreen"] = fullscreen;
	close();
}

void dgm::Window::toggleFullscreen() {
	auto size = getSize();
	close();
	fullscreen = !fullscreen;
	open(size, title, fullscreen);
}

void dgm::Window::changeResolution(const sf::Vector2u& newResolution) {
	close();
	open(newResolution, title, fullscreen);
}

sf::Image dgm::Window::getScreenshot() const {
	sf::Vector2u windowSize = window.getSize();
	sf::Texture texture;
	texture.create(windowSize.x, windowSize.y);
	texture.update(window);
	return texture.copyToImage();
}
