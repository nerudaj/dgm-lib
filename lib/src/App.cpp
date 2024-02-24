#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Error.hpp>
#include <iostream>

void dgm::App::updateTopState(size_t stateIdx, bool updateState, bool drawState)
{
    assert(!states.empty());

    auto&& state = states[stateIdx];

    if (stateIdx > 0
        && (state->shouldUpdateUnderlyingState()
            || state->shouldDrawUnderlyingState()))
    {
        updateTopState(
            stateIdx - 1,
            updateState && state->shouldUpdateUnderlyingState(),
            drawState && state->shouldDrawUnderlyingState());
    }

    if (updateState)
    {
        state->input();
        state->update();
    }
    if (drawState) state->draw();

    if (shouldPopStates()) performPostFrameCleanup();
}

void dgm::App::clearStack()
{
    while (!states.empty())
    {
        // ensure correct order of deinitialization
        states.pop_back();
    }
}

void dgm::App::performPostFrameCleanup()
{
    numberOfStatesToPop =
        std::clamp(numberOfStatesToPop, size_t(0), states.size());

    while (numberOfStatesToPop > 0)
    {
        --numberOfStatesToPop;
        states.pop_back();
    }

    if (not states.empty())
    {
        getTopState().restoreFocus(messageForRestore);
        messageForRestore.clear();
    }
}

void dgm::App::run()
{
    while (window.isOpen() && not states.empty())
    {
        window.beginDraw(getTopState().getClearColor());
        updateTopState(states.size() - 1);
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
