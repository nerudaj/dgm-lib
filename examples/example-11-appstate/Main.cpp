/*
This example showcases how to use dgm::App, how to create custom dgm::AppStates
and how to even use transparent app states.

It firsts creates a "loader" state - a red screen which swaps to "game" state
after one second. "Game" state alternates between blue and green background and
when you press ESC, it goes to "Pause" state. "Pause" state renders small
rectangle over the "game" state (which does not update during that time) and
when you press ESC, it goes back to "game" state.

Closing the window will exit the app.
*/
#include "DemoData.hpp"
#include "UI.hpp"
#include <DGM/dgm.hpp>

class OptionsState : public dgm::AppState
{
public:
    OptionsState(dgm::App& app, sf::Text& text)
        : dgm::AppState(app)
        , text(text)
        , buttons(computeLayout(
              { "back to menu", "back" },
              Alignment::Center,
              sf::Vector2f(app.window.getSize())))
    {
    }

public:
    void input() override
    {
        const auto mousePos =
            sf::Mouse::getPosition(app.window.getSfmlWindowContext());
        while (const auto event = app.window.pollEvent())
        {
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (dgm::Collision::basic(buttons[0].box, mousePos))
                    // This string will be passed to restoreStateImpl of a state
                    // this will return to. This allows you to implement
                    // messaging protocols between states
                    app.popState("jumpToMenu");
                // This will return either to PlayState or MainState, depending
                // on from where OptionState was entered
                else if (dgm::Collision::basic(buttons[1].box, mousePos))
                    app.popState();
            }
        }
    }

    void update() override {}

    void draw() override
    {
        renderButtons(buttons, text, app.window, sf::Color(128, 0, 0));
    }

private:
    sf::Text& text;
    std::vector<Button> buttons;
};

class PlayState : public dgm::AppState
{
public:
    PlayState(dgm::App& app, sf::Text& text)
        : dgm::AppState(
            app, dgm::AppStateConfig { .shouldDrawUnderlyingState = true })
        , text(text)
        , buttons(computeLayout(
              { "back to menu", "options" },
              Alignment::Center,
              sf::Vector2f(app.window.getSize())))
    {
    }

public:
    void input() override
    {
        const auto mousePos =
            sf::Mouse::getPosition(app.window.getSfmlWindowContext());
        while (const auto event = app.window.pollEvent())
        {
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (dgm::Collision::basic(buttons[0].box, mousePos))
                    app.popState();
                else if (dgm::Collision::basic(buttons[1].box, mousePos))
                    app.pushState<OptionsState>(text);
            }
        }
    }

    void update() override {}

    void draw() override
    {
        // Since underlying state is rendered (see AppStateConfig in
        // constructor) you will see a dimmed version of MainState below this
        // one
        renderButtons(buttons, text, app.window, sf::Color(128, 128, 128, 240));
    }

private:
    void restoreFocusImpl(const std::string& message) override
    {
        if (message == "jumpToMenu") app.popState();
    }

private:
    sf::Text& text;
    std::vector<Button> buttons;
};

class MainState : public dgm::AppState
{
public:
    MainState(dgm::App& app, sf::Text& text)
        : dgm::AppState(app)
        , text(text)
        , buttons(computeLayout(
              { "play", "options", "exit" },
              Alignment::Left,
              sf::Vector2f(app.window.getSize())))
    {
    }

public:
    void input() override
    {
        const auto mousePos =
            sf::Mouse::getPosition(app.window.getSfmlWindowContext());
        while (const auto event = app.window.pollEvent())
        {
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (dgm::Collision::basic(buttons[0].box, mousePos))
                    app.pushState<PlayState>(text);
                else if (dgm::Collision::basic(buttons[1].box, mousePos))
                    app.pushState<OptionsState>(text);
                else if (dgm::Collision::basic(buttons[2].box, mousePos))
                    app.exit();
            }
        }
    }

    void update() override {}

    void draw() override
    {
        renderButtons(buttons, text, app.window, sf::Color(128, 128, 0));
    }

private:
    sf::Text& text;
    std::vector<Button> buttons;
};

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: App", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& text = sf::Text(resmgr.get<sf::Font>("cruft.ttf"));
    text.setCharacterSize(72);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);

    // Reference to the window will be preserved inside app. You don't need to
    // pass it around yourself.
    auto&& app = dgm::App(window);

    // Note how you don't have to manually pass 'app' to pushState method.
    // You only push all other parameters for the constructor and 'app' will be
    // provided by the method itself. It uses concept library to check that
    // template derives from dgm::AppState and is constructible from 'app' and
    // provided parameters.
    app.pushState<MainState>(text);

    // Run will repeatedly call input, update and draw on active state until
    // there is a state present. One can exit the app by calling app.exit() from
    // a state or calling app.popState() until there are no more states left.
    app.run();

    return 0;
}
