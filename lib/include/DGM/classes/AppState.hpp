#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	class AppState {
	protected:
		App &app;

	public:
		/**
		 *  \brief Process input from Window::pollEvent()
		 */
		virtual void input() = 0;
		
		/**
		 *  \brief Main state logic
		 */
		virtual void update() = 0;
		
		/**
		 *  \brief Render state logic
		 * 
		 *  Only issue calls to window.draw in from within this function.
		 *  You should NOT call dgm::Window::beginDraw and dgm::Window::endDraw
		 *  as those will be called for you.
		 */
		virtual void draw() = 0;

		/**
		 *  This method is called when stack of states has been
		 *  popped and this state is now the top state.
		 * 
		 *  It is optional to override this method.
		 */
		virtual void restoreFocus() {}
		
		/**
		 *  Return color that will be used as window clear color
		 *  for the next frame. If you're OK with black (most use cases)
		 *  you don't have to override this method.
		 */
		virtual [[nodiscard]] sf::Color getClearColor() const {
			return sf::Color::Black;
		}

		virtual [[nodiscard]] bool isTransparent() const noexcept = 0;

		AppState(dgm::App& app) : app(app) {}
		AppState(AppState&&) = delete;
		AppState(AppState&) = delete;
		virtual ~AppState() = default;
	};
};