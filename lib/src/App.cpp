#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>

void dgm::App::clearStack()
{
    while (not states.empty())
        states.pop();
}

void dgm::App::performPostFrameCleanup()
{
    while (numberOfStatesToPop > 0)
    {
        numberOfStatesToPop--;
        states.pop();
        numberOfStatesToPop;
    }

    if (not states.empty()) topState().restoreFocus();
}

void dgm::App::takeScreenshot()
{
    auto&& capture = window.getScreenshot();
    screenshot.loadFromImage(capture);
    screenshotSprite.setTexture(screenshot);
    screenshotSprite.setOrigin(sf::Vector2f(capture.getSize() / 2u));
}

void dgm::App::run()
{
    while (window.isOpen() && not states.empty())
    {
        auto& top = topState();

        top.input();
        top.update();

        window.beginDraw(top.getClearColor());

        if (top.isTransparent())
        {
            screenshotSprite.setPosition(
                window.getWindowContext().getView().getCenter());
            window.draw(screenshotSprite);
        }

        top.draw();

        window.endDraw();

        if (shouldPopStates()) performPostFrameCleanup();

        time.reset();
    }
}

dgm::App::App(dgm::Window& window)
    : window(window), outbuf("stdout.txt"), errbuf("stderr.txt")
{
    if (outbuf.rdbuf() == nullptr || errbuf.rdbuf() == nullptr)
        throw dgm::GeneralException("Cannot redirect stdout/stderr to file");

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
