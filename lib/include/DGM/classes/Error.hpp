#pragma once

#include <expected>
#ifndef ANDROID
#include <format>
#include <stacktrace>
#endif
#include <stdexcept>
#include <string>

namespace dgm
{
    class [[nodiscard]] Exception : public std::runtime_error
    {
    public:
#ifdef ANDROID
        Exception(const std::string& message)
            : std::runtime_error("Error message: " + message)
        {
        }
#else
        Exception(
            const std::string& message,
            std::stacktrace trace = std::stacktrace::current())
            : std::runtime_error(std::format(
                  "Error message: {}\n\nStacktrace: \n{}",
                  message,
                  std::to_string(trace)))
        {
        }
#endif
    };

    class [[nodiscard]] Error final
    {
    public:
        Error(const std::string& message)
            : message(message)
#ifndef ANDROID
            , trace(std::stacktrace::current())
#endif
        {
        }

    public:
        const std::string& getMessage() const noexcept
        {
            return message;
        }

#ifndef ANDROID
        const std::stacktrace& getTrace() const noexcept
        {
            return trace;
        }
#endif

    private:
        std::string message;
#ifndef ANDROID
        std::stacktrace trace;
#endif
    };

    using ExpectedSuccess = std::expected<std::true_type, Error>;
} // namespace dgm