#include <DGM/classes/Controller.hpp>

void dgm::Controller::bindCode(const int code, sf::Keyboard::Key key, sf::Mouse::Button btn) {
	if (bindings.count(code) > 0) {
		throw dgm::EnvironmentException("Trying to redefine dgm::Controller binding with code: " + std::to_string(code));
	}

	bindings[code] = { false, key != sf::Keyboard::KeyCount, key, btn };
}

bool dgm::Controller::keyPressed(const int code) {
	bool pressed = bindings.at(code).isKey
		? sf::Keyboard::isKeyPressed(bindings[code].key)
		: sf::Mouse::isButtonPressed(bindings[code].btn);
	return pressed
		? not bindings[code].released
		: (bindings[code].released = false); // assignment is on purpose
}

void dgm::Controller::bindKeyboardKey(const int code, sf::Keyboard::Key key) {
	bindCode(code, key, sf::Mouse::Button::ButtonCount);
}

void dgm::Controller::bindMouseButton(const int code, sf::Mouse::Button btn) {
	bindCode(code, sf::Keyboard::KeyCount, btn);
}
