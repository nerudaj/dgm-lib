#pragma once

#include <expected>
#include <format>
#include <stacktrace>
#include <stdexcept>
#include <string>

namespace dgm
{
    class Exception : public std::runtime_error
    {
    public:
        Exception(
            const std::string& message,
            std::stacktrace trace = std::stacktrace::current())
            : std::runtime_error(std::format(
                  "Error message: {}\n\nStacktrace: \n{}",
                  message,
                  std::to_string(trace)))
        {
        }
    };

    using ErrorMessage = std::string;
    class [[nodiscard]] Error final
    {
    public:
        Error(const std::string& message)
            : message(message), trace(std::stacktrace::current())
        {
        }

    public:
        const std::string& getMessage() const noexcept
        {
            return message;
        }

        const std::stacktrace& getTrace() const noexcept
        {
            return trace;
        }

    private:
        std::string message;
        std::stacktrace trace;
    };

    using ExpectedSuccess = std::expected<std::true_type, ErrorMessage>;
} // namespace dgm