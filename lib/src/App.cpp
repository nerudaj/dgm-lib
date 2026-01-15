#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Error.hpp>
#include <SFML/System/Err.hpp>
#include <iostream>

dgm::App::App(dgm::Window& window)
    : window(window)
#ifndef ANDROID
    , outbuf("stdout.txt")
    , errbuf("stderr.txt")
#endif
{
#ifndef ANDROID
    if (outbuf.rdbuf() == nullptr || errbuf.rdbuf() == nullptr)
        throw dgm::Exception("Cannot redirect stdout/stderr to file");

    stdoutBackup = std::cout.rdbuf();
    stderrBackup = std::cerr.rdbuf();
    sferrBackup = sf::err().rdbuf();

    std::cout.rdbuf(outbuf.rdbuf());
    std::cerr.rdbuf(errbuf.rdbuf());
    sf::err().rdbuf(errbuf.rdbuf());
#endif

    time.reset();
}

dgm::App::~App()
{
    clearStack();

#ifndef ANDROID
    std::cout.rdbuf(stdoutBackup);
    std::cerr.rdbuf(stderrBackup);
    sf::err().rdbuf(sferrBackup);
#endif
}

void dgm::App::run()
{
    while (window.isOpen() && not states.empty())
    {
        updateState(states.size() - 1);

        window.clear(getTopState().getClearColor());

        drawState(
            states.size() - 1,
            sf::View { sf::Vector2f(window.getSize() / 2u),
                       sf::Vector2f(window.getSize()) });
        window.display();

        performScheduledCleanup();

        time.reset();
    }
}

void dgm::App::pushStateInternal(std::unique_ptr<AppState> state)
{
    statesToPush.push_back(std::move(state));

    // If this is true, this is the pushState of the original state (before all
    // other pushes happened)
    if (pushNestingCounter == 1)
    {
        // since the latest push state was the first to be pushed into
        // statesToPush we need to proceed from the back and push it to the
        // regular states
        while (!statesToPush.empty())
        {
            if (!states.empty()) getTopState().loseFocus();
            states.push_back(std::move(statesToPush.back()));
            statesToPush.pop_back();
        }
    }
}

void dgm::App::updateState(size_t stateIdx, bool shouldUpdateState)
{
    assert(!states.empty());
    assert(stateIdx < states.size());

    auto&& state = states[stateIdx];

    if (stateIdx > 0 && state->shouldUpdateUnderlyingState())
    {
        updateState(
            stateIdx - 1,
            shouldUpdateState && state->shouldUpdateUnderlyingState());
    }

    if (shouldUpdateState)
    {
        state->input();
        state->update();
    }
}

void dgm::App::drawState(
    size_t stateIdx, const sf::View& view, bool shouldDrawState)
{
    assert(!states.empty());
    assert(stateIdx < states.size());

    auto&& state = states[stateIdx];

    if (stateIdx > 0 && state->shouldDrawUnderlyingState())
    {
        drawState(
            stateIdx - 1,
            view,
            shouldDrawState && state->shouldDrawUnderlyingState());
    }

    if (shouldDrawState)
    {
        window.setView(view);
        state->draw();
    }
}

void dgm::App::performScheduledCleanup()
{
    while (scheduledCleanup != ScheduledCleanup::None)
    {
        if (scheduledCleanup == ScheduledCleanup::Pop)
            popStateInternal();
        else if (scheduledCleanup == ScheduledCleanup::Exit)
            clearStack();
    }
}

void dgm::App::popStateInternal()
{
    scheduledCleanup = ScheduledCleanup::None;
    states.pop_back();
    auto messageCopy = messageForRestore;
    messageForRestore.clear();
    if (!states.empty()) getTopState().restoreFocus(messageCopy);
}

void dgm::App::clearStack()
{
    while (!states.empty())
    {
        // ensure correct order of deinitialization
        states.pop_back();
    }

    scheduledCleanup = ScheduledCleanup::None;
}
