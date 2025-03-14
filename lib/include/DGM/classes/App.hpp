#pragma once

#include <DGM/classes/Compatibility.hpp>
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
         */
        template<IsDerivedFromAppState T, class... Args>
            requires std::constructible_from<T, dgm::App&, Args...>
        void pushState(Args&&... args)
        {
            pushNestingCounter++;

            // T constructor might invoke pushState
            // in which case pushNestingCounter will be > 1 at this point
            try
            {
                pushStateInternal(
                    std::make_unique<T>(*this, std::forward<Args>(args)...));
            }
            catch (const std::exception&)
            {
                pushNestingCounter--;
                throw;
            }

            pushNestingCounter--;
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
        constexpr void popState(const std::string& message = "")
        {
            assert(scheduledCleanup == ScheduledCleanup::None);
            messageForRestore = message;
            scheduledCleanup = ScheduledCleanup::Pop;
        }

        /**
         *  \brief Exit the app with proper deinitialization of states
         *
         *  The actual termination will happen at the end of the frame
         *  so input/update/draw will be performed.
         */
        constexpr void exit() noexcept
        {
            assert(scheduledCleanup != ScheduledCleanup::Pop);
            scheduledCleanup = ScheduledCleanup::Exit;
        }

    private:
        void pushStateInternal(std::unique_ptr<AppState> state);

        void updateState(size_t stateIdx, bool updateState = true);

        void drawState(size_t stateIdx, bool drawState = true);

        /**
         *  \brief Get reference to top state on the stack
         */
        NODISCARD_RESULT dgm::AppState& getTopState() noexcept(
            noexcept(states.back()) && noexcept(states.back().get()))
        {
            assert(not states.empty());
            return *states.back().get();
        }

        void performScheduledCleanup();
        void popStateInternal();
        void clearStack();

    private:
        enum class [[nodiscard]] ScheduledCleanup
        {
            None,
            Pop,
            Exit
        };

        std::ofstream outbuf;
        std::ofstream errbuf;
        std::streambuf* stdoutBackup = nullptr;
        std::streambuf* stderrBackup = nullptr;
        std::streambuf* sferrBackup = nullptr;
        std::deque<std::unique_ptr<AppState>> states;
        ScheduledCleanup scheduledCleanup = ScheduledCleanup::None;
        std::string messageForRestore = "";
        int pushNestingCounter = 0;
        std::deque<std::unique_ptr<AppState>> statesToPush;
    };
} // namespace dgm
