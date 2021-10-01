#pragma once

#include <DGM\dgm.hpp>

namespace dgm {
	class AppState {
	protected:
		App *app;

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
		 *  \brief Initialize the state object
		 */
		virtual bool init() = 0;

		void setAppPointer(App *app) {
			AppState::app = app;
		}
	};
};