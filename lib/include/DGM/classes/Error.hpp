#pragma once

#include <stdexcept>

namespace dgm {
	class GeneralException : public std::runtime_error {
	public:
		GeneralException(const std::string &message) : std::runtime_error(message) {}
	};
	
	class ResourceException : public std::runtime_error {
	public:
		ResourceException(const std::string &message) : std::runtime_error(message) {}
	};
	
	class EnvironmentException : public std::runtime_error {
	public:
		EnvironmentException(const std::string &message) : std::runtime_error(message) {}
	};

	class ShellException : public std::runtime_error {
	public:
		ShellException(const std::string& msg) : std::runtime_error(msg) {}
	};
}