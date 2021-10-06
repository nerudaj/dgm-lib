#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>

#include <fstream>

class AppEnvironment {
private:
	bool status = false;
	std::ofstream outbuf;
	std::ofstream errbuf;

public:
	bool isLoggingEnabled() const {
		return status;
	}

	AppEnvironment() : outbuf("stdout.txt"), errbuf("stderr.txt") {
		status = outbuf.rdbuf() != nullptr && errbuf.rdbuf() != nullptr;
		std::cout.rdbuf(outbuf.rdbuf());
		std::cerr.rdbuf(errbuf.rdbuf());
	}
};

const AppEnvironment ENVIRONMENT;

void dgm::App::pushState(dgm::AppState * state) {
	states.push(state);

	topState()->setAppPointer(this);
	if (not topState()->init()) {
		std::cerr << "ERROR:App: State was not initialized properly.\n";
		popState();
	}
}

void dgm::App::popState() {
	states.pop();
	scheduleCleanup = true;
}

dgm::AppState * dgm::App::topState() {
	if (states.empty()) return nullptr;

	return states.top();
}

void dgm::App::init() {
	scheduleCleanup = false;
}

void dgm::App::deinit() {
	while (not states.empty()) {
		delete topState();
		popState();
	}
}

void dgm::App::run() {
	while (window.isOpen() && not states.empty()) {
		AppState *top = topState();

		top->input();
		top->update();
		top->draw();
		time.reset();

		if (scheduleCleanup) {
			delete top;
			scheduleCleanup = false;
		}
	}
}

dgm::App::App() {
	if (!ENVIRONMENT.isLoggingEnabled()) {
		exit(1);
	}

	init();
	time.reset();
}

dgm::App::~App() {
	deinit();
}
