#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

struct Reporter
{
    bool inputCalled = false;
    bool updateCalled = false;
    bool drawCalled = false;
    bool ctorCalled = false;
    bool dtorCalled = false;
    unsigned drawCallCount = 0;
    bool screenshotTaken = false;
};

enum class TestableStateBehaviour : std::size_t
{
    Default,
    PopState,
    PopState3,
    ExitApp
};

class TestableWindow_App : public dgm::Window
{
protected:
    Reporter* reporter = nullptr;

public:
    virtual void draw(sf::Drawable&)
    {
        reporter->drawCallCount++;
    }

    virtual [[nodiscard]] sf::Image getScreenshot() const
    {
        reporter->screenshotTaken = true;
        sf::Image image;
        image.create(1, 1, nullptr);
        return image;
    }

    virtual [[nodiscard]] bool isOpen() const
    {
        return true;
    }

    TestableWindow_App(Reporter* reporter) : reporter(reporter) {}
};

class TestableState : public dgm::AppState
{
protected:
    Reporter* reporter = nullptr;
    TestableStateBehaviour behaviour = TestableStateBehaviour::Default;
    float dt = 0.f;

public:
    // Dědí se přes AppState.
    virtual void input() override
    {
        reporter->inputCalled = true;
    }

    virtual void update() override
    {
        reporter->updateCalled = true;

        switch (behaviour)
        {
        case TestableStateBehaviour::PopState:
            app.popState();
            break;
        case TestableStateBehaviour::PopState3:
            app.popState();
            app.popState(2);
            break;
        case TestableStateBehaviour::ExitApp:
            app.exit();
            break;
        default:
            break;
        }
    }

    virtual void draw() override
    {
        reporter->drawCalled = true;
        dt = app.time.getDeltaTime();
    }

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    TestableState(
        dgm::App& app, Reporter* reporter, TestableStateBehaviour behaviour)
        : dgm::AppState(app), reporter(reporter), behaviour(behaviour)
    {
        reporter->ctorCalled = true;
    }

    virtual ~TestableState()
    {
        reporter->dtorCalled = true;
    }
};

class TestableStateWithRestore : public TestableState
{
public:
    virtual void restoreFocus() override
    {
        app.exit();
    }

    TestableStateWithRestore(
        dgm::App& app, Reporter* reporter, TestableStateBehaviour behaviour)
        : TestableState(app, reporter, behaviour)
    {
    }
};

class TestableTransparentState : public dgm::AppState
{
public:
    virtual void input() {}

    virtual void update()
    {
        app.exit();
    }

    virtual void draw() {}

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return true;
    }

    TestableTransparentState(dgm::App& app) : dgm::AppState(app) {}
};

TEST_CASE("Cout/cerr restoration", "App")
{
    auto stdoutbackup = std::cout.rdbuf();

    Reporter reporter;
    TestableWindow_App window(&reporter);
    dgm::App* app = new dgm::App(window);

    REQUIRE(std::cout.rdbuf() != nullptr);
    REQUIRE(std::cout.rdbuf() != stdoutbackup);

    delete app;

    REQUIRE(std::cout.rdbuf() != nullptr);
    REQUIRE(std::cout.rdbuf() == stdoutbackup);
}

TEST_CASE("Push/pop state", "App")
{
    Reporter reporter;
    TestableWindow_App window(&reporter);

    dgm::App app(window);
    app.pushState<TestableState>(&reporter, TestableStateBehaviour::PopState);
    app.run();

    REQUIRE(reporter.ctorCalled);
    REQUIRE(reporter.inputCalled);
    REQUIRE(reporter.updateCalled);
    REQUIRE(reporter.drawCalled);
    REQUIRE(reporter.dtorCalled);
}

TEST_CASE("Exit app", "App")
{
    Reporter reporter1, reporter2, reporter3, wr;
    TestableWindow_App window(&wr);

    dgm::App app(window);
    app.pushState<TestableState>(&reporter1, TestableStateBehaviour::Default);
    app.pushState<TestableState>(&reporter2, TestableStateBehaviour::Default);
    app.pushState<TestableState>(&reporter3, TestableStateBehaviour::ExitApp);
    app.run();

    REQUIRE(reporter1.ctorCalled);
    REQUIRE(reporter1.dtorCalled);

    REQUIRE(reporter2.ctorCalled);
    REQUIRE(reporter2.dtorCalled);

    REQUIRE(reporter3.ctorCalled);
    REQUIRE(reporter3.inputCalled);
    REQUIRE(reporter3.updateCalled);
    REQUIRE(reporter3.drawCalled);
    REQUIRE(reporter3.dtorCalled);
}

TEST_CASE("Transparent appstate", "App")
{
    Reporter reporter;
    TestableWindow_App window(&reporter);

    dgm::App app(window);
    app.pushState<TestableState>(&reporter, TestableStateBehaviour::Default);
    app.pushState<TestableTransparentState>();
    app.run();

    REQUIRE(reporter.screenshotTaken);
    REQUIRE(reporter.drawCallCount == 1);
}

TEST_CASE("Exit on focus restore", "App")
{
    Reporter reporter, reporter2;
    TestableWindow_App window(&reporter);

    dgm::App app(window);
    app.pushState<TestableStateWithRestore>(
        &reporter, TestableStateBehaviour::Default);
    app.pushState<TestableState>(&reporter2, TestableStateBehaviour::PopState);
    app.run();

    REQUIRE(reporter.dtorCalled);
    REQUIRE(reporter2.dtorCalled);
}

TEST_CASE("Aggregation of popState", "App")
{
    Reporter reporter, reporter2;
    TestableWindow_App window(&reporter);

    dgm::App app(window);
    app.pushState<TestableState>(&reporter, TestableStateBehaviour::Default);
    app.pushState<TestableState>(&reporter, TestableStateBehaviour::Default);
    app.pushState<TestableState>(
        &reporter, TestableStateBehaviour::PopState3); // this pops three states
                                                       // in one frame
    app.run();

    // The fact that app.run() returns is enough for this test
    REQUIRE(true);
}
