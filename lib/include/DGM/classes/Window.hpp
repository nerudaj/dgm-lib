#pragma once

#include <DGM/classes/Camera.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace dgm
{
    struct [[nodiscard]] WindowSettings final
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
    class [[nodiscard]] Window final
    {
    public:
        explicit Window(const WindowSettings& settings)
        {
            open(settings);
        }

        Window(
            const sf::Vector2u& resolution,
            const std::string& title,
            bool fullscreen)
        {
            open(resolution, title, fullscreen);
        }

        Window(Window&&) = delete;
        Window(Window&) = delete;

        ~Window()
        {
            if (isOpen()) std::ignore = close();
        }

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
        NODISCARD_RESULT WindowSettings close();

        /**
         *  \brief Pop the next event from the front of the FIFO event queue, if
        any, and return it
         *
         * This function is not blocking: if there's no pending event then
         * it will return a `std::nullopt`. Note that more than one event
         * may be present in the event queue, thus you should always call
         * this function in a loop to make sure that you process every
         * pending event.
         * \code
         * while (const std::optional event = window.pollEvent())
        {
            // process event...
        }
         * \endcode
         *
         * \return The event, otherwise `std::nullopt` if no events are pending
         */
        NODISCARD_RESULT inline auto pollEvent()
        {
            return window.pollEvent();
        }

        /**
         * \brief Set the current view from the camera to the window
         *
         * \param[in] camera  A camera instance with update view
         *
         *  This is a shorthand for calling
         *  window.getSfmlWindowContext().setView(camera.getCurrentView())
         */
        inline void setViewFromCamera(const dgm::Camera& camera)
        {
            window.setView(camera.getCurrentView());
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
        NODISCARD_RESULT virtual bool isOpen() const
        {
            return window.isOpen();
        }

        /**
         *  \brief Test whether window is in fullscreen mode
         */
        NODISCARD_RESULT constexpr bool isFullscreen() const noexcept
        {
            return fullscreen;
        }

        /**
         *  \brief Get dimensions of the window
         *
         *  \return Width and height of window render area in pixels
         */
        NODISCARD_RESULT sf::Vector2u getSize() const
        {
            return window.getSize();
        }

        /**
         *  \brief Get handle to internal instance of sf::RenderWindow
         *
         *  Use this method whether you need something from sf::RenderWindow API
         *  not supported directly by this class
         */
        NODISCARD_RESULT sf::RenderWindow& getSfmlWindowContext() noexcept
        {
            return window;
        }

        /**
         *  \brief Get handle to internal instance of sf::RenderWindow
         *
         *  Use this method whether you need something from sf::RenderWindow API
         *  not supported directly by this class
         */
        NODISCARD_RESULT const sf::RenderWindow&
        getSfmlWindowContext() const noexcept
        {
            return window;
        }

        /**
         *  \brief Get title text of the window
         */
        NODISCARD_RESULT constexpr const std::string& getTitle() const noexcept
        {
            return title;
        }

        /**
         *  \brief Prepare for drawing by clearing the Window with color
         *
         *  \param[in]  color  Color to clear window to (Default: Black)
         */
        void clear(const sf::Color& color = sf::Color::Black)
        {
            window.clear(color);
        }

        /**
         *  \brief Draw object on the Window
         *
         *  \param[in]  drawable  Reference to drawable object
         */
        void draw(sf::Drawable& drawable)
        {
            window.draw(drawable);
        }

        /**
         *  \brief Finish drawing by rendering Window to screen
         */
        void display()
        {
            window.display();
        }

        NODISCARD_RESULT sf::Image getScreenshot() const;

    protected:
        sf::RenderWindow window;
        bool fullscreen = false;
        std::string title = "";
        std::int64_t style = sf::Style::Default;
    };
}; // namespace dgm