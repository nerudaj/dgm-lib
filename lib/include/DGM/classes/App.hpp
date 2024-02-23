#pragma once

#include <DGM/classes/Time.hpp>
#include <DGM/classes/Window.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cassert>
#include <concepts>
#include <fstream>
#include <stack>

namespace dgm
{
    class AppState;

    template<class T>
    concept IsDerivedFromAppState = std::derived_from<T, dgm::AppState>;

    class [[nodiscard]] App final
    {

    public:
        explicit App(dgm::Window& window);
        App(App&&) = delete;
        App& operator=(App&&) = delete;
        App(const App&) = delete;
        App& operator=(const App&) = delete;
        ~App();

    public:
        Window& window;
        Time time; ///< Time between frames

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
         *
         *  \warn This method must NOT be called from any ctor
         *  derived from dgm::AppState. If you want to construct a
         *  state and immediately switch to a new one, postpone call
         *  to pustState into update method.
         */
        template<IsDerivedFromAppState T, class... Args>
            requires std::constructible_from<T, dgm::App&, Args...>
        void pushState(Args&&... args)
        {
            if (!states.empty()) states.top()->loseFocus();
            states.push(
                std::make_unique<T>(*this, std::forward<Args>(args)...));
        }

        /**
         *  \brief Run the main app loop
         *
         *  \details When no state is on stack, this
         *  methods returns.
         */
        void run();

        /**
         *  \brief Removes top N states from the app stack
         *
         *  The actual removal will happen at the end of the frame
         *  so input/update/draw will be performed.
         *
         *  Calling this method multiple times per frame aggregates the
         *  number of states that will be popped
         */
        constexpr void popState(
            const std::string& message = "", const unsigned count = 1) noexcept
        {
            messageForRestore = message;
            numberOfStatesToPop += count;
        }

        /**
         *  \brief Exit the app with proper deinitialization of states
         *
         *  The actual termination will happen at the end of the frame
         *  so input/update/draw will be performed.
         */
        void exit() noexcept(noexcept(states.size()))
        {
            // when called from active state, size of stack is off by one
            numberOfStatesToPop = states.size() + 1;
        }

    protected:
        void updateTopState(bool updateState = true, bool drawState = true);

        /**
         *  \brief Get reference to top state on the stack
         */
        [[nodiscard]] dgm::AppState& topState() noexcept(
            noexcept(states.top()) && noexcept(states.top().get()))
        {
            assert(not states.empty());
            return *states.top().get();
        }

        void clearStack();
        void performPostFrameCleanup();

        [[nodiscard]] constexpr bool shouldPopStates() const noexcept
        {
            return numberOfStatesToPop > 0;
        }

    protected:
        std::ofstream outbuf;
        std::ofstream errbuf;
        std::streambuf* stdoutBackup = nullptr;
        std::streambuf* stderrBackup = nullptr;
        std::stack<std::unique_ptr<AppState>> states;
        std::size_t numberOfStatesToPop = 0;
        std::string messageForRestore = "";
    };
} // namespace dgm
