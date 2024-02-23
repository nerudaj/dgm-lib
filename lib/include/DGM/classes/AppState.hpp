#pragma once

#include <DGM/classes/App.hpp>
#include <DGM/classes/AppStateConfig.hpp>

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
        void restoreFocus()
        {
            hasFocus = true;
            restoreFocusImpl();
        }

        void loseFocus()
        {
            hasFocus = false;
            loseFocusImpl();
        }

    public:
        [[nodiscard]] constexpr const sf::Color& getClearColor() const noexcept
        {
            return config.clearColor;
        }

        [[nodiscard]] constexpr bool
        shouldUpdateUnderlyingState() const noexcept
        {
            return config.shouldUpdateUnderlyingState;
        }

        [[nodiscard]] constexpr bool shouldDrawUnderlyingState() const noexcept
        {
            return config.shouldDrawUnderlyingState;
        }

    protected:
        virtual void restoreFocusImpl() {}

        virtual void loseFocusImpl() {}

    protected:
        App& app;
        const AppStateConfig config;
        bool hasFocus = true;
    };
}; // namespace dgm