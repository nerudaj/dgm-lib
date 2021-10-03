/**
 *  \file Controller.hpp
 *  \author doomista
 */
 
#pragma once

#include <map>
#include <DGM/dgm.hpp>

namespace dgm {
	class AbstractController {
	public:
		virtual bool keyPressed(const int code) = 0;
		
		virtual void releaseKey(const int code) = 0;
	};

	class Binding {
	public:
		bool released;
		bool isKey;
		sf::Keyboard::Key key;
		sf::Mouse::Button btn;
	};

	class Controller : public AbstractController {
	protected:
		std::map<int, Binding> bindings;

		void bindCode(const int code, sf::Keyboard::Key key, sf::Mouse::Button btn);

	public:

		/**
		 * \brief Test whether particular input code is pressed
		 * 
		 * Input code might be mapped to keyboard key XOR mouse button.
		 * If input code was previously released using releaseKey then
		 * physical key/button must first be released, this function called
		 * and only then will this function return TRUE again.
		 */
		[[nodiscard]] bool keyPressed(const int code);

		/**
		 * \brief Marks input as released
		 *
		 * \details Only works in conjuction with simpler version of
		 * keyPressed(). Once an action is marked as released then
		 * keyPressed() will return FALSE until point where user
		 * had released the input physically and then pressed it again.
		 * With this, one can emulate sf::Event::keyPressed behaviour.
		 *
		 * \note In order to function properly, keyPressed should be called
		 * every frame (to ensure that controller will notice the released key)
		 */
		void releaseKey(const int code) noexcept {
			bindings[code].released = true;
		}

		/**
		 *  Bind numerical input code to keyboard key
		 * 
		 *  \warn There must not be binding of the same input code to both
		 *  keyboard key and mouse button. Exception is thrown in such case.
		 */
		void bindKeyboardKey(const int code, sf::Keyboard::Key key);

		/**
		 *  Bind numerical input code to mouse button
		 *
		 *  \warn There must not be binding of the same input code to both
		 *  keyboard key and mouse button. Exception is thrown in such case.
		 */
		void bindMouseButton(const int code, sf::Mouse::Button btn);

		Controller() {}
		~Controller() {}
	};
}