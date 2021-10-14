#pragma once

#include <DGM/classes/Window.hpp>
#include <DGM/classes/Time.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <stack>
#include <fstream>
#include <cassert>
#include <concepts>

namespace dgm {
	class AppState;

	template<class T>
	concept IsDerivedFromAppState = std::derived_from<T, dgm::AppState>;

	class App {
	public:
		Window& window;
		Time time; ///< Time between frames

	protected:
		std::ofstream outbuf;
		std::ofstream errbuf;
		std::streambuf* stdoutBackup = nullptr;
		std::streambuf* stderrBackup = nullptr;
		std::stack<std::unique_ptr<AppState>> states;
		bool scheduledDestructionOfTopState = false;
		bool scheduledDestructionOfApp = false;
		sf::Texture screenshot;
		sf::Sprite screenshotSprite;

	protected:
		/**
		 *  \brief Get reference to top state on the stack
		 */
		[[nodiscard]] dgm::AppState& topState() noexcept {
			assert(not states.empty());
			return *states.top().get();
		}

		void clearStack();
		void performPostFrameCleanup();
		void takeScreenshot();

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
		template<IsDerivedFromAppState T, class ... Args>
		void pushState(Args ... args) {
			states.push(std::make_unique<T>(*this, args...));
			if (topState().isTransparent()) takeScreenshot();
		}
		
		/**
		 *  \brief Run the main app loop
		 *
	     *  \details When no state is on stack, this
		 *  methods returns.
		 */
		void run();
		
		/**
		 *  \brief Remove top state from app stack
		 * 
		 *  The actual removal will happen at the end of the frame
		 *  so input/update/draw will be performed.
		 */
		void popState() noexcept {
			scheduledDestructionOfTopState = true;
		}

		/**
		 *  \brief Exit the app with proper deinitialization of states
		 * 
		 *  The actual termination will happen at the end of the frame
		 *  so input/update/draw will be performed.
		 */
		void exit() {
			scheduledDestructionOfTopState = true;
			scheduledDestructionOfApp = true;
		}

		App(dgm::Window& window);
		App(App&&) = delete;
		App(App&) = delete;
		~App();
	};
}
