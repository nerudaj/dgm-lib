#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

namespace dgm
{
    struct WindowSettings
    {
        sf::Vector2u resolution = { 0u, 0u };
        std::string title = "";
        bool useFullscreen = false;
    };

    /**
     *  \brief Wrapper around sf::RenderWindow
     *
     *  Class provides method for easier window management.
     */
    class Window
    {
    protected:
        sf::RenderWindow window;
        bool fullscreen = false;
        std::string title = "";
        sf::Uint32 style = sf::Style::Default;

    public:
        /**
         *  \brief Open Window using a Ini config object
         *
         *  The \p config object should contain a [Window] section
         *  and defined keys 'fullscreen', 'width', 'height' and 'title'.
         *  If [Window] section is not defined, defaults will be used.
         *  Defaults are: 1280x720, no title, no fullscreen.
         */
        void open(const WindowSettings& settings)
        {
            open(settings.resolution, settings.title, settings.useFullscreen);
        }

        /**
         *  \brief Open a window
         *
         *  \param[in]  resolution  Target window resolution
         *  \param[in]  title       Caption in systray
         *  \param[in]  fullscreen  Whether to start in fullscreen
         */
        void open(
            const sf::Vector2u& resolution,
            const std::string& title,
            bool fullscreen);

        /**
         *  \brief Close the Window and return configuration of closed window
         */
        [[nodiscard]] WindowSettings close();

        /**
         *  \brief Poll next event from SFML
         *
         *  \param[in]  event  Destination variable for polled event
         *  \return TRUE if event was stored, FALSE if there were no more
         * events.
         *
         *  This function mimicks the behaviour of sf::RenderWindow::pollEvent
         */
        [[nodiscard]] bool pollEvent(sf::Event& event)
        {
            return window.pollEvent(event);
        }

        /**
         *  \brief Toggle window between fullscreen and windowed mode
         *
         *  \note Call to this function will close and re-open the window!
         */
        void toggleFullscreen();

        /**
         *  \brief Change resolution of a Window
         *
         *  \warn  This will close and re-open the Window!
         */
        void changeResolution(const sf::Vector2u& newResolution);

        /**
         *  \brief Test whether window is still open
         */
        [[nodiscard]] virtual bool isOpen() const
        {
            return window.isOpen();
        }

        /**
         *  \brief Test whether window is in fullscreen mode
         */
        [[nodiscard]] constexpr bool isFullscreen() const noexcept
        {
            return fullscreen;
        }

        /**
         *  \brief Get dimensions of the window
         *
         *  \return Width and height of window render area in pixels
         */
        [[nodiscard]] sf::Vector2u getSize() const
        {
            return window.getSize();
        }

        /**
         *  \brief Get handle to internal instance of sf::RenderWindow
         *
         *  Use this method whether you need something from sf::RenderWindow API
         *  not supported directly by this class
         */
        [[nodiscard]] sf::RenderWindow& getWindowContext()
        {
            return window;
        }

        /**
         *  \brief Get handle to internal instance of sf::RenderWindow
         *
         *  Use this method whether you need something from sf::RenderWindow API
         *  not supported directly by this class
         */
        [[nodiscard]] const sf::RenderWindow& getWindowContext() const
        {
            return window;
        }

        /**
         *  \brief Get title text of the window
         */
        [[nodiscard]] const std::string& getTitle() const noexcept
        {
            return title;
        }

        /**
         *  \brief Prepare for drawing by clearing the Window with color
         *
         *  \param[in]  color  Color to clear window to (Default: Black)
         */
        void beginDraw(const sf::Color& color = sf::Color::Black)
        {
            window.clear(color);
        }

        /**
         *  \brief Draw object on the Window
         *
         *  \param[in]  drawable  Reference to drawable object
         */
        virtual void draw(sf::Drawable& drawable)
        {
            window.draw(drawable);
        }

        /**
         *  \brief Finish drawing by rendering Window to screen
         */
        void endDraw()
        {
            window.display();
        }

        [[nodiscard]] virtual sf::Image getScreenshot() const;

        [[nodiscard]] Window() = default;

        [[nodiscard]] explicit Window(const WindowSettings& settings)
        {
            open(settings);
        }

        [[nodiscard]] Window(
            const sf::Vector2u& resolution,
            const std::string& title,
            bool fullscreen)
        {
            open(resolution, title, fullscreen);
        }

        Window(Window&&) = delete;
        Window(Window&) = delete;

        virtual ~Window()
        {
            if (isOpen()) std::ignore = close();
        }
    };
}; // namespace dgm