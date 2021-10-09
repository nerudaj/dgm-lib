#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>

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

void dgm::App::exit() {
	// Pop state at the top
	// it will be deleted via scheduled cleanup
	popState();

	// Pop and delete the rest
	while (not states.empty()) {
		delete states.top();
		states.pop();
	}
}

dgm::App::App(dgm::Window &window) : window(window), outbuf("stdout.txt"), errbuf("stderr.txt") {
	if (outbuf.rdbuf() == nullptr || errbuf.rdbuf() == nullptr) {
		throw dgm::GeneralException("Cannot redirect stdout/stderr to file");
	}

	stdoutBackup = std::cout.rdbuf();
	stderrBackup = std::cerr.rdbuf();

	std::cout.rdbuf(outbuf.rdbuf());
	std::cerr.rdbuf(errbuf.rdbuf());

	time.reset();
}

dgm::App::~App() {
	while (not states.empty()) {
		delete topState();
		popState();
	}

	std::cout.rdbuf(stdoutBackup);
	std::cerr.rdbuf(stderrBackup);
}
