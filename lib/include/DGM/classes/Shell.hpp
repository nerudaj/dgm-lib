#pragma once

#include <DGM/classes/RingBuffer.hpp>

#include <map>
#include <vector>
#include <stdexcept>
#include <functional>
#include <string>

namespace dgm {
	/**
	 *  \brief Base class for creating shell modules.
	 *
	 *  To implement a shell module, one has to subclass this
	 *  class, implement `getDescription` method and in ctor
	 *  call ctor of this class (providing name of new module
	 *  as a parameter) and also add all supported actions
	 *  using addAction method.
	 */
	class ShellModule {
	public:
		using Arguments = std::vector<std::string>;

	protected:
		struct Action {
			unsigned argc; /// Number of arguments required by action
			std::string description; /// Description of action for help dialog
			std::function<std::string(const Arguments&)> callback; /// Callback to execute. Returns stdout of the action
		};

		/**
		 *  \brief Add action to a module
		 *
		 *  Use this method to add unique actions to a particular module.
		 *  This method checks uniqueness of each action name.
		 *  "help" is always present by default.
		 */
		void addAction(const std::string &name, const Action& action);

	private:
		std::map<std::string, Action> actions;
		std::string name;

		std::string printHelp() const;

	public:
		/**
		 *  \brief Execute action on module with arguments
		 *
		 *  \details Returns stdout of the command, stderr is returned via exception.
		 *  Catch ShellException to read the stderr.
		 *
		 *  Arguments will be stripped of the action name before passing
		 *  them to particular action.
		 */
		std::string execute(const Arguments& args);

		[[nodiscard]] const std::string& getName() const noexcept {
			return name;
		}

		/**
		 *  \brief Description method for help dialoague
		 */
		virtual std::string getDescription() const = 0;

		ShellModule(const std::string &name);
		virtual ~ShellModule() {}
	};

	/**
	 *  \brief Module for manipulating variables at runtime.
	 *
	 *  Each variable has to be added via bindVariable method.
	 *  Currently only int, float and string variables are
	 *  supported.
	 */
	class ShellModuleVars : public ShellModule {
	public:
		class Variable {
		public:
			virtual void set(const std::string& value) = 0;
			virtual std::string getType() const = 0;
			virtual std::string getValue() const = 0;

			virtual ~Variable() {}
		};

	private:
		std::map<std::string, Variable*> variables;

		std::string formatVariable(const std::string& name) const;
	public:
		template<typename T>
		void bindVariable(const std::string& name, T* ptr);

		virtual std::string getDescription() const override;

		ShellModuleVars();
		~ShellModuleVars();
	};

	class Shell {
	private:
		std::map<std::string, ShellModule*> modules;

		std::string printGlobalHelp() const;

		RingBuffer<std::string, 10> history;

	public:
		/**
		 *  \brief Executes command in the shell
		 *
		 *  \return Returns stdout of the command. If command fails,
		 *  it throws a ShellException.
		 */
		std::string execute(const std::string &command);

		/**
		 *  \brief Add a module to Shell
		 *
		 *  Name of the module (provided as parameter to ShellModule
		 *  ctor) will be used to access the module in the shell.
		 */
		void install(ShellModule& module);

		/**
		 *  \brief Getter for the shell history
		 *
		 *  History last N commands, where N is up to 'size'.
		 *  front() element of history is oldest remembered item,
		 *  last() is the most recent command.
		 */
		const RingBuffer<std::string, 10>& getHistory() const {
			return history;
		}

		Shell();
		Shell(Shell &&other) = delete;
		Shell(const Shell &other) = delete;
		Shell &operator=(const Shell &other) = delete;
		~Shell();
	};
}