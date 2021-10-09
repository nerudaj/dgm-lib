#include <catch.hpp>
#include <DGM/dgm.hpp>

struct Reporter {
	bool destructorCalled = false;
	bool inputCalled = false;
	bool updateCalled = false;
	bool drawCalled = false;
	bool initCalled = false;
};

enum class TestableStateBehaviour : std::size_t {
	Default,
	PopState,
	ExitApp
};

class TestableState : public dgm::AppState {
protected:
	Reporter& reporter;
	TestableStateBehaviour behaviour = TestableStateBehaviour::Default;
	float dt = 0.f;

public:
	// Dědí se přes AppState.
	virtual void input() override {
		reporter.inputCalled = true;
	}
	virtual void update() override {
		reporter.updateCalled = true;
		app->popState();
	}
	virtual void draw() override {
		reporter.drawCalled = true;
		dt = app->time.getDeltaTime();
	}
	virtual bool init() override {
		reporter.initCalled = true;
		return true;
	}

	TestableState(Reporter& reporter, TestableStateBehaviour behaviour) : reporter(reporter), behaviour(behaviour) {}
	virtual ~TestableState() {
		reporter.destructorCalled = true;
	}
};

class TestableWindow_App : public dgm::Window {
public:
	virtual [[nodiscard]] bool isOpen() const { return true; }
};

TEST_CASE("Cout/cerr restoration", "App") {
	auto stdoutbackup = std::cout.rdbuf();

	TestableWindow_App window;
	dgm::App *app = new dgm::App(window);

	REQUIRE(std::cout.rdbuf() != nullptr);
	REQUIRE(std::cout.rdbuf() != stdoutbackup);

	delete app;

	REQUIRE(std::cout.rdbuf() != nullptr);
	REQUIRE(std::cout.rdbuf() == stdoutbackup);
}

TEST_CASE("Push/pop state", "App") {
	TestableWindow_App window;
	Reporter reporter;

	dgm::App app(window);

	app.pushState(new TestableState(reporter, TestableStateBehaviour::PopState));
	REQUIRE(reporter.initCalled);

	app.run();

	REQUIRE(reporter.inputCalled);
	REQUIRE(reporter.updateCalled);
	REQUIRE(reporter.drawCalled);
	REQUIRE(reporter.destructorCalled);
}

TEST_CASE("Exit app", "App") {
	TestableWindow_App window;
	Reporter reporter1, reporter2, reporter3;

	dgm::App app(window);
	app.pushState(new TestableState(reporter1, TestableStateBehaviour::Default));
	app.pushState(new TestableState(reporter2, TestableStateBehaviour::Default));
	app.pushState(new TestableState(reporter3, TestableStateBehaviour::ExitApp));
	app.run();

	REQUIRE(reporter1.destructorCalled);
	REQUIRE(reporter2.destructorCalled);
	REQUIRE(reporter3.destructorCalled);
}
