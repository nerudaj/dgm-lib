﻿#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>

struct Reporter
{
    bool inputCalled = false;
    bool updateCalled = false;
    bool drawCalled = false;
    bool ctorCalled = false;
    bool dtorCalled = false;
    bool hasFocus = false;
};

enum class TestableStateBehaviour : std::size_t
{
    Default,
    PopState,
    PopState3,
    ExitApp,
    ExitOnRestore
};

class TestableState : public dgm::AppState
{
protected:
    Reporter* reporter = nullptr;
    TestableStateBehaviour behaviour = TestableStateBehaviour::Default;

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
    }

    virtual void restoreFocusImpl()
    {
        reporter->hasFocus = true;
        if (behaviour == TestableStateBehaviour::ExitOnRestore) app.exit();
    }

    virtual void loseFocusImpl()
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
    }

    virtual ~TestableState()
    {
        reporter->dtorCalled = true;
    }
};

TEST_CASE("Cout/cerr restoration", "App")
{
    auto stdoutbackup = std::cout.rdbuf();

    dgm::Window window;
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
        app.run();

        REQUIRE(reporter.ctorCalled);
        REQUIRE(reporter.inputCalled);
        REQUIRE(reporter.updateCalled);
        REQUIRE(reporter.drawCalled);
        REQUIRE(reporter.dtorCalled);
    }

    SECTION("Exit clears the whole app stack")
    {
        Reporter r1, r2, r3;

        app.pushState<TestableState>(&r1, TestableStateBehaviour::Default);
        app.pushState<TestableState>(&r2, TestableStateBehaviour::Default);
        app.pushState<TestableState>(&r3, TestableStateBehaviour::ExitApp);
        app.run();

        REQUIRE(r1.ctorCalled);
        REQUIRE(r1.dtorCalled);

        REQUIRE(r2.ctorCalled);
        REQUIRE(r2.dtorCalled);

        REQUIRE(r3.ctorCalled);
        REQUIRE(r3.inputCalled);
        REQUIRE(r3.updateCalled);
        REQUIRE(r3.drawCalled);
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

    SECTION("Can pop multiple states at once")
    {
        Reporter r1, r2, r3;

        app.pushState<TestableState>(&r1);
        app.pushState<TestableState>(&r2);
        app.pushState<TestableState>(
            &r3,
            TestableStateBehaviour::PopState3); // this pops three states
                                                // in one frame
        app.run();

        // The fact that app.run() returns is enough for this test
        REQUIRE(true);
    }

    SECTION("After pushing multiple states, non-top states lose focus")
    {
        Reporter r1, r2, r3;

        app.pushState<TestableState>(&r1);
        app.pushState<TestableState>(&r2);
        app.pushState<TestableState>(&r3);

        REQUIRE_FALSE(r1.hasFocus);
        REQUIRE_FALSE(r2.hasFocus);
        REQUIRE(r3.hasFocus);
    }
}