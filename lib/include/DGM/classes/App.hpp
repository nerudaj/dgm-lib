#pragma once

#include <DGM/classes/Window.hpp>
#include <DGM/classes/Time.hpp>

#include <stack>
#include <fstream>

namespace dgm {
	class AppState;

	class App {
	public:
		Window& window;
		Time time; ///< Time between frames

	protected:
		std::ofstream outbuf;
		std::ofstream errbuf;
		std::streambuf* stdoutBackup = nullptr;
		std::streambuf* stderrBackup = nullptr;
		std::stack<AppState*> states;
		bool scheduleCleanup = false; // Schedule memory cleanup of state popped during current run

	public:

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
		 *  \brief Run the main app loop
		 *
	     *  \details When no state is on stack, this
		 *  methods returns.
		 */
		void run();
		
		/**
		 *  \brief Exit the app with proper deinitialization of states
		 */
		void exit();

		App(dgm::Window& window);
		~App();
	};
}
