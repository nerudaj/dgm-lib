#pragma once

#include <DGM\dgm.hpp>

#include <stack>

namespace dgm {
	class AppState;

	class App {
	protected:
		std::stack<AppState*> states;
		bool scheduleCleanup; // Schedule memory cleanup of state popped during current run

	public:
		Window window; ///< Window context of the app
		Time time; ///< Time between frames

		/**
		 *  \brief Add new AppState to App stack
		 *
		 *  \param [in] state Newly created AppState object pointer
		 *
		 *  \details First, pointer to this will be
		 *  passed to the state, then init method is
		 *  called. If init succeeds, the state is
		 *  pushed to app stack.
		 */
		void pushState(dgm::AppState *state);
		
		/**
		 *  \brief Remove top state from app stack
		 */
		void popState();
		
		/**
		 *  \brief Get pointer to top state on app stack
		 */
		dgm::AppState *topState();
		
		/**
		 *  \brief Initialize the object
		 *
		 *  \details Called automatically from constructor
		 */
		void init();
		
		/**
		 *  \brief Deinitialize the object
		 *
		 *  \details Called automaticall from destructor
		 */
		void deinit();
		
		/**
		 *  \brief Run the main app loop
		 *
	     *  \details When no state is on stack, this
		 *  methods returns.
		 */
		void run();
		
		App();
		~App();
	};
};