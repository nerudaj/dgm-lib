#pragma once

#include <DGM/classes/App.hpp>
#include <DGM/classes/AppStateConfig.hpp>
#include <DGM/classes/Compatibility.hpp>

namespace dgm
{
    class [[nodiscard]] AppState
    {
    public:
        constexpr explicit AppState(
            dgm::App& app,
            const AppStateConfig& config = AppStateConfig()) noexcept
            : app(app), config(config)
        {
        }

        AppState(AppState&&) = delete;
        AppState& operator=(AppState&&) = delete;
        AppState(const AppState&) = delete;
        AppState& operator=(const AppState&) = delete;
        virtual ~AppState() = default;

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
         *  You should NOT call dgm::Window::clear and dgm::Window::display
         *  as those will be called for you.
         */
        virtual void draw() = 0;

        void restoreFocus(const std::string& message)
        {
            hasFocus = true;
            restoreFocusImpl(message);
        }

        void loseFocus()
        {
            hasFocus = false;
            loseFocusImpl();
        }

    public:
        NODISCARD_RESULT constexpr const sf::Color&
        getClearColor() const noexcept
        {
            return config.clearColor;
        }

        NODISCARD_RESULT constexpr bool
        shouldUpdateUnderlyingState() const noexcept
        {
            return config.shouldUpdateUnderlyingState;
        }

        NODISCARD_RESULT constexpr bool
        shouldDrawUnderlyingState() const noexcept
        {
            return config.shouldDrawUnderlyingState;
        }

    protected:
        /**
         *  This method is called when stack of states has been
         *  popped and this state is now the top state.
         *
         *  It is optional to override this method.
         *
         *  The string parameter is a message previously passed
         *  to dgm::App::popState method.
         */
        virtual void restoreFocusImpl(const std::string&) {}

        virtual void loseFocusImpl() {}

    protected:
        App& app;
        const AppStateConfig config;
        bool hasFocus = true;
    };
}; // namespace dgm