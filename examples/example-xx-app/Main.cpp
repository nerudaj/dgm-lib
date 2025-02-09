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

#include <DGM/dgm.hpp>

class StateLoader;
class StateGame;
class StatePause;

// This state just waits for a second before it pushes another state to a stack
class StateLoader : public dgm::AppState
{
protected:
    sf::Time timer;

public:
    virtual void input() override {}

    virtual void update() override;

    virtual void draw() override {}

    StateLoader(dgm::App& app, sf::Time duration)
        : dgm::AppState(
            app,
            dgm::AppStateConfig {
                .clearColor = sf::Color::Red,
            })
        , timer(duration)
    {
    }
};

// This state waits either for closing the window or for pressing the ESCAPE,
// where it invokes the pause state It alternates between blue and green
// background
class StateGame : public dgm::AppState
{
protected:
    float timer = 0.f;
    sf::Color backgroundColors[2] = { sf::Color::Green, sf::Color::Blue };
    int backgroundColorIndex = 0;

public:
    virtual void input() override;

    virtual void update() override
    {
        timer += app.time.getDeltaTime(); // float based use of dgm::Time
        if (timer > 1.f)
        { // 1 second
            timer = 0.f;
            backgroundColorIndex = 1 - backgroundColorIndex;
        }
    }

    virtual void draw() override {}

    StateGame(dgm::App& app)
        : dgm::AppState(
            app,
            dgm::AppStateConfig {
                // TODO: this is no longer possible
                .clearColor = backgroundColors[backgroundColorIndex],
            })
    {
    }
};

// This is the pause state. It is supposed to be transparent, only rendering
// small rectangle over the "Game" state. It waits for ESC to return back to
// game state or for user closing the app
class StatePause : public dgm::AppState
{
protected:
    dgm::Rect rect = dgm::Rect(100.f, 100.f, 100.f, 300.f);

public:
    virtual void input() override;

    virtual void update() override {}

    virtual void draw() override
    {
        rect.debugRender(app.window);
    }

    StatePause(dgm::App& app)
        : dgm::AppState(
            app,
            dgm::AppStateConfig {
                .shouldDrawUnderlyingState = true,
            })
    {
    }
};

int main()
{
    dgm::Window window({ 1280, 720 }, "Example: App", false);
    dgm::App app(window);

    // Note how you don't have to manually pass 'app' to pushState method.
    // You only push all other parameters for the constructor and 'app' will be
    // provided by the method itself. It uses concept library to check that
    // template derives from dgm::AppState and is constructible from 'app' and
    // provided parameters.
    app.pushState<StateLoader>(sf::seconds(1.f));
    app.run();

    return 0;
}

// These methods had to be defined here so they know how constructors look like.
void StateLoader::update()
{
    timer -= app.time.getElapsed(); // time since last frame
    if (timer <= sf::Time::Zero) app.pushState<StateGame>();
}

void StateGame::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                app.pushState<StatePause>();
        }
        else if (event.type == sf::Event::Closed)
            app.exit();
    }
}

void StatePause::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape) app.popState();
        }
        else if (event.type == sf::Event::Closed)
            app.exit();
    }
}