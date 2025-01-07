#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

struct Reporter
{
    bool inputCalled = false;
    bool updateCalled = false;
    bool drawCalled = false;
    bool ctorCalled = false;
    bool dtorCalled = false;
    bool hasFocus = false;
    std::string restoreMessage = "";
};

enum class TestableStateBehaviour : std::size_t
{
    Default,
    PopState,
    PopState3,
    PopWithMessage,
    PushExitState,
    PopInUpdateAndRestoreWithMessage,
    ExitApp,
    ExitOnRestore,
    PushInCtor,
    PushInCtor_AnotherPushInCtor
};

class TestableState : public dgm::AppState
{
protected:
    Reporter* reporter = nullptr;
    TestableStateBehaviour behaviour = TestableStateBehaviour::Default;

public:
    // Dědí se přes AppState.
    void input() override
    {
        reporter->inputCalled = true;
    }

    void update() override
    {
        reporter->updateCalled = true;

        switch (behaviour)
        {
        case TestableStateBehaviour::PopState:
            app.popState();
            break;
        case TestableStateBehaviour::PopState3:
            app.popState();
            app.popState("");
            break;
        case TestableStateBehaviour::PopWithMessage:
            app.popState("test message");
            break;
        case TestableStateBehaviour::ExitApp:
            app.exit();
            break;
        case TestableStateBehaviour::PushExitState:
            app.pushState<TestableState>(
                reporter, TestableStateBehaviour::ExitApp);
            break;
        case TestableStateBehaviour::PopInUpdateAndRestoreWithMessage:
            app.popState("test message");
            break;
        default:
            break;
        }
    }

    void draw() override
    {
        reporter->drawCalled = true;
    }

    void restoreFocusImpl(const std::string& message)
    {
        reporter->hasFocus = true;
        reporter->restoreMessage = message;
        if (behaviour == TestableStateBehaviour::ExitOnRestore)
            app.exit();
        else if (
            behaviour
                == TestableStateBehaviour::PopInUpdateAndRestoreWithMessage
            && message == "test message")
        {
            app.popState(message);
        }
    }

    void loseFocusImpl()
    {
        reporter->hasFocus = false;
    }

    TestableState(
        dgm::App& app,
        Reporter* reporter,
        TestableStateBehaviour behaviour = TestableStateBehaviour::Default,
        const dgm::AppStateConfig& config = dgm::AppStateConfig())
        : dgm::AppState(app, config), reporter(reporter), behaviour(behaviour)
    {
        reporter->ctorCalled = true;
        reporter->hasFocus = hasFocus;

        if (behaviour == TestableStateBehaviour::PushInCtor)
        {
            app.pushState<TestableState>(
                reporter, TestableStateBehaviour::ExitApp);
        }
        else if (
            behaviour == TestableStateBehaviour::PushInCtor_AnotherPushInCtor)
        {
            app.pushState<TestableState>(
                reporter, TestableStateBehaviour::PushInCtor);
        }
    }

    virtual ~TestableState()
    {
        reporter->dtorCalled = true;
    }
};

class StateThatThrowsInCtor : public dgm::AppState
{
public:
    StateThatThrowsInCtor(dgm::App& app) : dgm::AppState(app)
    {
        throw std::exception();
    }

    void input() override {}

    void update() override {}

    void draw() override {}
};

TEST_CASE("Cout/cerr restoration", "App")
{
    auto stdoutbackup = std::cout.rdbuf();

    auto&& window = dgm::Window({ 0, 0 }, "", false);
    dgm::App* app = new dgm::App(window);

    REQUIRE(std::cout.rdbuf() != nullptr);
    REQUIRE(std::cout.rdbuf() != stdoutbackup);

    delete app;

    REQUIRE(std::cout.rdbuf() != nullptr);
    REQUIRE(std::cout.rdbuf() == stdoutbackup);
}

TEST_CASE("App")
{
    Reporter reporter;
    auto&& window = dgm::Window({ 1u, 1u }, "", false);
    dgm::App app(window);

    SECTION("input/update/draw are called even with pop in the middle")
    {
        app.pushState<TestableState>(
            &reporter, TestableStateBehaviour::PopState);
        REQUIRE(reporter.ctorCalled);

        app.run();

        REQUIRE(reporter.inputCalled);
        REQUIRE(reporter.updateCalled);
        REQUIRE(reporter.drawCalled);
        REQUIRE(reporter.dtorCalled);
    }

    SECTION("Exit clears the whole app stack")
    {
        Reporter r1, r2, r3;

        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        REQUIRE(r1.ctorCalled);

        app.pushState<TestableState>(&r2, TestableStateBehaviour::Default);
        REQUIRE(r2.ctorCalled);

        app.pushState<TestableState>(&r3, TestableStateBehaviour::ExitApp);
        REQUIRE(r3.ctorCalled);

        app.run();

        REQUIRE(r3.inputCalled);
        REQUIRE(r3.updateCalled);
        REQUIRE(r3.drawCalled);

        REQUIRE(r1.dtorCalled);
        REQUIRE(r2.dtorCalled);
        REQUIRE(r3.dtorCalled);
    }

    SECTION("When shouldDrawUnderlyingState, draw is correctly invoked N times")
    {
        Reporter r1, r2, r3;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        app.pushState<TestableState>(
            &r2,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig {
                .shouldDrawUnderlyingState = true,
            });
        app.pushState<TestableState>(
            &r3,
            TestableStateBehaviour::ExitApp,
            dgm::AppStateConfig {
                .shouldDrawUnderlyingState = true,
            });
        app.run();

        REQUIRE_FALSE(r1.inputCalled);
        REQUIRE_FALSE(r1.updateCalled);
        REQUIRE(r1.drawCalled);

        REQUIRE_FALSE(r2.inputCalled);
        REQUIRE_FALSE(r2.updateCalled);
        REQUIRE(r2.drawCalled);

        REQUIRE(r3.inputCalled);
        REQUIRE(r3.updateCalled);
        REQUIRE(r3.drawCalled);
    }

    SECTION(
        "When shouldUpdateUnderlyingState, input/update are correctly invoked "
        "N times")
    {
        Reporter r1, r2, r3;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        app.pushState<TestableState>(
            &r2,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = true,
            });
        app.pushState<TestableState>(
            &r3,
            TestableStateBehaviour::ExitApp,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = true,
            });
        app.run();

        REQUIRE(r1.inputCalled);
        REQUIRE(r1.updateCalled);
        REQUIRE_FALSE(r1.drawCalled);

        REQUIRE(r2.inputCalled);
        REQUIRE(r2.updateCalled);
        REQUIRE_FALSE(r2.drawCalled);

        REQUIRE(r3.inputCalled);
        REQUIRE(r3.updateCalled);
        REQUIRE(r3.drawCalled);
    }

    SECTION("Mixed shouldUpdate/DrawUnderlyingState")
    {
        // State 1 - no transparency
        // State 2 - full transparency
        // State 3 - partial transparency (only update)
        // --> State 1 should be updated, but not drawn
        Reporter r1, r2, r3, r4, r5;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        app.pushState<TestableState>(
            &r2,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = true,
                .shouldDrawUnderlyingState = true,
            });
        app.pushState<TestableState>(
            &r3,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = true,
                .shouldDrawUnderlyingState = false,
            });
        app.pushState<TestableState>(
            &r4,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = false,
                .shouldDrawUnderlyingState = true,
            });
        app.pushState<TestableState>(
            &r5,
            TestableStateBehaviour::ExitApp,
            dgm::AppStateConfig {
                .shouldUpdateUnderlyingState = true,
                .shouldDrawUnderlyingState = true,
            });
        app.run();

        REQUIRE_FALSE(r1.inputCalled);
        REQUIRE_FALSE(r1.updateCalled);
        REQUIRE_FALSE(r1.drawCalled);

        REQUIRE_FALSE(r2.inputCalled);
        REQUIRE_FALSE(r2.updateCalled);
        REQUIRE_FALSE(r2.drawCalled);

        REQUIRE_FALSE(r3.inputCalled);
        REQUIRE_FALSE(r3.updateCalled);
        REQUIRE(r3.drawCalled);

        REQUIRE(r4.inputCalled);
        REQUIRE(r4.updateCalled);
        REQUIRE(r4.drawCalled);

        REQUIRE(r5.inputCalled);
        REQUIRE(r5.updateCalled);
        REQUIRE(r5.drawCalled);
    }

    SECTION("Can exit from restoreFocusImpl")
    {
        Reporter reporter2;

        app.pushState<TestableState>(
            &reporter, TestableStateBehaviour::ExitOnRestore);
        app.pushState<TestableState>(
            &reporter2, TestableStateBehaviour::PopState);
        app.run();

        REQUIRE(reporter.dtorCalled);
        REQUIRE(reporter2.dtorCalled);
    }

    SECTION("After pushing multiple states, non-top states lose focus")
    {
        Reporter r1, r2, r3;

        app.pushState<TestableState>(&r1);
        REQUIRE(r1.hasFocus);

        app.pushState<TestableState>(&r2);
        REQUIRE_FALSE(r1.hasFocus);
        REQUIRE(r2.hasFocus);

        app.pushState<TestableState>(&r3);

        REQUIRE_FALSE(r1.hasFocus);
        REQUIRE_FALSE(r2.hasFocus);
        REQUIRE(r3.hasFocus);
    }

    SECTION("Message is passed down during pop")
    {
        Reporter r1, r2;

        app.pushState<TestableState>(&r1, TestableStateBehaviour::PopState);
        app.pushState<TestableState>(
            &r2, TestableStateBehaviour::PopWithMessage);
        app.run();

        REQUIRE(r1.restoreMessage == "test message");
    }

    SECTION("Push from current top state works")
    {
        Reporter r1;

        app.pushState<TestableState>(
            &r1, TestableStateBehaviour::PushExitState);
        app.run();
        // should push state with exit behaviour on the top of the stack
        // right after that, the stack should be popped

        // The fact that app.run() returns is enough for this test
        REQUIRE(true);
    }

    SECTION("Popping from restoreFocusImpl works")
    {
        Reporter r1, r2, r3, r4;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::ExitApp);
        app.pushState<TestableState>(
            &r2, TestableStateBehaviour::PopInUpdateAndRestoreWithMessage);
        app.pushState<TestableState>(
            &r3, TestableStateBehaviour::PopInUpdateAndRestoreWithMessage);
        app.pushState<TestableState>(
            &r4,
            TestableStateBehaviour::Default,
            dgm::AppStateConfig { .shouldUpdateUnderlyingState = true });
        app.run();
        REQUIRE(true);
    }

    SECTION(
        "Destroying app while states are on a stack will properly destroy them")
    {
        Reporter r1, r2;

        {
            auto&& a = dgm::App(window);
            a.pushState<TestableState>(&r1);
            a.pushState<TestableState>(&r2);
        }

        REQUIRE(r1.dtorCalled);
        REQUIRE(r2.dtorCalled);
    }

    SECTION(
        "App state pushed from constructor of another app state is on the top")
    {
        Reporter r1;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::PushInCtor);
        app.run();
        REQUIRE(true);
    }

    SECTION(
        "App state pushed from constructor of another app state is on the top "
        "(multiple pushes)")
    {
        Reporter r1;
        app.pushState<TestableState>(
            &r1, TestableStateBehaviour::PushInCtor_AnotherPushInCtor);
        app.run();
        REQUIRE(true);
    }

    SECTION(
        "Focus is lost appropriately even if some pushState throws exception")
    {
        Reporter r1, r2;
        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        try
        {
            app.pushState<StateThatThrowsInCtor>();
        }
        catch (...)
        {
        }
        app.pushState<TestableState>(&r2, TestableStateBehaviour::ExitApp);
        app.run(); // A part of the bug was that the extra state never got
                   // pushed so
        // the app would keep on running in this test

        REQUIRE_FALSE(r1.hasFocus);
        REQUIRE(r2.hasFocus);
    }
}
