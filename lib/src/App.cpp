#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Error.hpp>
#include <iostream>

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

void dgm::App::run()
{
    while (window.isOpen() && not states.empty())
    {
        window.beginDraw(getTopState().getClearColor());
        updateState(states.size() - 1);
        window.endDraw();

        performScheduledCleanup();

        time.reset();
    }
}

void dgm::App::updateState(
    size_t stateIdx, bool shouldUpdateState, bool shouldDrawState)
{
    assert(!states.empty());
    assert(stateIdx < states.size());

    auto&& state = states[stateIdx];

    if (stateIdx > 0
        && (state->shouldUpdateUnderlyingState()
            || state->shouldDrawUnderlyingState()))
    {
        updateState(
            stateIdx - 1,
            shouldUpdateState && state->shouldUpdateUnderlyingState(),
            shouldDrawState && state->shouldDrawUnderlyingState());
    }

    if (shouldUpdateState)
    {
        state->input();
        state->update();
    }
    if (shouldDrawState) state->draw();
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
