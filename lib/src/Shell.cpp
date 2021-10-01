#include <DGM/dgm.hpp>
#include <fstream>

using dgm::Shell;
using dgm::ShellModule;
using dgm::ShellException;

const std::string TAB(4, ' ');

/* ShellModule */
void dgm::ShellModule::addAction(const std::string& name, const Action& action) {
	if (actions.find(name) != actions.end()) {
		throw ShellException("Trying to add action '" + name + "' to module '" + ShellModule::name + " but it is already here");
	}

	actions[name] = action;
}
std::string ShellModule::printHelp() const {
	std::string result = "Available actions for command '" + name + "':\n";

	for (auto& kv : actions) {
		result += TAB + kv.second.description + '\n';
	}

	return result + "\n";
}

std::string ShellModule::execute(const Arguments& args) {
	if (args.size() < 1) {
		throw ShellException("Too few arguments, expected 'var <action> [params..]'");
	}

	auto action = args[0];

	if (actions.find(action) == actions.end()) {
		throw ShellException("Invalid action. Try '" + name + " help' to list available actions");
	}

	Arguments actionArgs(args.begin() + 1, args.end());

	if (actionArgs.size() != actions.at(action).argc) {
		throw ShellException("Wrong number of arguments for '" + name + " " + action + "'");
	}

	return actions.at(action).callback(actionArgs);
}

ShellModule::ShellModule(const std::string& name) : name(name) {
	addAction("help", { 0, "help: Prints this text", [this](const Arguments& args) {
		return printHelp();
	} });
}

/* ShellModuleVars */
class FloatVariable : public dgm::ShellModuleVars::Variable {
	float* ptr;

public:
	virtual void set(const std::string& value) final override {
		*ptr = std::stof(value);
	}
	virtual std::string getType() const final override { return "float"; }
	virtual std::string getValue() const final override { return std::to_string(*ptr); }

	FloatVariable(float* ptr) : ptr(ptr) {}
};

class IntVariable : public dgm::ShellModuleVars::Variable {
	int* ptr;

public:
	virtual void set(const std::string& value) final override {
		*ptr = std::stol(value);
	}
	virtual std::string getType() const final override { return "int"; }
	virtual std::string getValue() const final override { return std::to_string(*ptr); }

	IntVariable(int* ptr) : ptr(ptr) {}
};

class StringVariable : public dgm::ShellModuleVars::Variable {
	std::string* ptr;

public:
	virtual void set(const std::string& value) final override {
		*ptr = value;
	}
	virtual std::string getType() const final override { return "std::string"; }
	virtual std::string getValue() const final override { return *ptr; }

	StringVariable(std::string* ptr) : ptr(ptr) {}
};

std::string dgm::ShellModuleVars::formatVariable (const std::string &name) const {
	return "CONST(" + variables.at(name)->getType() + ", " + name + ", " + variables.at(name)->getValue() + ");\n";
}

template<>
void dgm::ShellModuleVars::bindVariable(const std::string &name, float *ptr) {
	variables[name] = new FloatVariable(ptr);
}

template<>
void dgm::ShellModuleVars::bindVariable(const std::string &name, int *ptr) {
	variables[name] = new IntVariable(ptr);
}

template<>
void dgm::ShellModuleVars::bindVariable(const std::string& name, std::string *ptr) {
	variables[name] = new StringVariable(ptr);
}

std::string dgm::ShellModuleVars::getDescription() const {
	return "Interact with bound variables";
}

dgm::ShellModuleVars::ShellModuleVars() : ShellModule("var") {
	addAction("get", {1, "get <varname>: Get value of variable", [this] (const Arguments &args) {
		auto varname = args[0];

		if (variables.find(varname) == variables.end()) {
			throw ShellException("Variable named " + varname + " was not bound to this shell.");
		}

		return formatVariable(varname);
	}});

	addAction("set", {2, "set <varname> <value>: Set value of variable", [this] (const Arguments &args) {
		auto varname = args[0];
		auto value = args[1];

		if (variables.find(varname) == variables.end()) {
			throw ShellException("Variable named " + varname + " was not bound to this shell.");
		}

		try {
			variables.at(varname)->set(value);
		} catch(...) {
			throw ShellException("Could not set value for variable " + varname + ": value '" + value + "' is invalid.");
		}

		return varname + " now equals to " + value + "\n";
	}});

	addAction("list", {0, "list: List names of bound variables", [this] (const Arguments &args) {
		std::string result = "List of bound variables:\n";
		for (auto pair : variables) {
			result += TAB + pair.first + "\n";
		}
		return result;
	}});

	addAction("export", {1, "export <filename>: Export variables to target file in form of C++ header", [this] (const Arguments &args) {
		auto filename = args[0];

		std::ofstream save(filename);

		save << "#pragma once" << std::endl << std::endl;
		for (auto &kv : variables) {
			save << formatVariable(kv.first);
		}

		save.close();
		return "Variables were save to " + filename + "\n";
	}});
}

dgm::ShellModuleVars::~ShellModuleVars() {
	for (auto &pair : variables) {
		delete pair.second;
		pair.second = nullptr;
	}
}

/* Shell */
std::string Shell::printGlobalHelp() const {
	std::string result = "Available commands:\n";
	result += TAB + "help: Prints this text.\n";

	for (auto &pair : modules) {
		auto module = pair.second;
		result += TAB + module->getName() + ": " + module->getDescription() + "\n";
	}

	result += "\n";
	result += "Type '<modulename> help' to get help for a particular command.\n";

	return result;
}

std::string Shell::execute(const std::string &command) {
	history.pushBack(command);

	auto args = Strings::split(' ', command);
	if (args.size() == 0) {
		throw ShellException("Shell::execute(): Nothing to execute!");
	}

	auto module = args[0];
	args.erase(args.begin());

	if (module == "help") {
		return printGlobalHelp();
	}

	if (modules.find(module) == modules.end()) {
		throw ShellException("Shell::execute(): Command " + module + " is not recognized! Try 'help' to list available commands.");
	}

	return modules.at(module)->execute(args);
}

void Shell::install(ShellModule& module) {
	if (modules.find(module.getName()) != modules.end()) {
		throw ShellException("Cannot install module '" + module.getName() + "'. It is already installed.");
	}

	modules[module.getName()] = &module;
}

Shell::Shell() {}

Shell::~Shell() {}