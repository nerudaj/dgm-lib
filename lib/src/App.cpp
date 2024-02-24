#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Error.hpp>
#include <iostream>

void dgm::App::updateTopState(bool updateState, bool drawState)
{
    assert(!states.empty());
    if (states.empty()) return;

    auto state = std::move(states.top());
    states.pop();

    if (state->shouldUpdateUnderlyingState()
        || state->shouldDrawUnderlyingState())
    {
        updateTopState(
            updateState && state->shouldUpdateUnderlyingState(),
            drawState && state->shouldDrawUnderlyingState());
    }

    if (updateState)
    {
        state->input();
        state->update();
    }
    if (drawState) state->draw();

    states.push(std::move(state));

    if (shouldPopStates()) performPostFrameCleanup();
}

void dgm::App::clearStack()
{
    while (not states.empty())
        states.pop();
}

void dgm::App::performPostFrameCleanup()
{
    while (numberOfStatesToPop > 0 && !states.empty())
    {
        numberOfStatesToPop--;
        states.pop();
    }

    numberOfStatesToPop = 0;
    if (not states.empty())
    {
        topState().restoreFocus(messageForRestore);
        messageForRestore.clear();
    }
}

void dgm::App::run()
{
    while (window.isOpen() && not states.empty())
    {
        window.beginDraw(states.top()->getClearColor());
        updateTopState();
        window.endDraw();
        time.reset();
    }
}

dgm::App::App(dgm::Window& window)
    : window(window), outbuf("stdout.txt"), errbuf("stderr.txt")
{
    if (outbuf.rdbuf() == nullptr || errbuf.rdbuf() == nullptr)
        throw dgm::Exception("Cannot redirect stdout/stderr to file");

    stdoutBackup = std::cout.rdbuf();
    stderrBackup = std::cerr.rdbuf();

    std::cout.rdbuf(outbuf.rdbuf());
    std::cerr.rdbuf(errbuf.rdbuf());

    time.reset();
}

dgm::App::~App()
{
    clearStack();

    std::cout.rdbuf(stdoutBackup);
    std::cerr.rdbuf(stderrBackup);
}
