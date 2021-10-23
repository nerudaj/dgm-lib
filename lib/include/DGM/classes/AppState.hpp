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
		 */
		virtual void draw() = 0;

		/**
		 *  This method is called when stack of states has been
		 *  popped and this state is now the top state.
		 * 
		 *  It is optional to override this method.
		 */
		virtual void restoreFocus() {}
		
		virtual [[nodiscard]] bool isTransparent() const noexcept = 0;

		AppState(dgm::App& app) : app(app) {}
		AppState(AppState&&) = delete;
		AppState(AppState&) = delete;
		virtual ~AppState() = default;
	};
};