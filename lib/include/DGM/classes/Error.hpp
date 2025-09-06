#pragma once

#include <expected>
#ifdef __cpp_lib_stacktrace
#include <stacktrace>
#endif
#include <stdexcept>
#include <string>

namespace dgm
{
    class [[nodiscard]] Exception : public std::runtime_error
    {
    public:
        Exception(
            const std::string& message,
#ifdef __cpp_lib_stacktrace
            std::stacktrace trace = std::stacktrace::current())
#else
            std::string trace = "Trace is not available with your compiler")
#endif
            : std::runtime_error(
                "Error message: " + message + "\n\nStacktrace: " +
#ifdef __cpp_lib_stacktrace
                      std::to_string(trace)))
#else
                      trace))
#endif
        {
        }
    };

    class [[nodiscard]] Error final
    {
    public:
        Error(const std::string& message)
            : message(message)
#ifdef __cpp_lib_stacktrace
            , trace(std::stacktrace::current())
#endif
        {
        }

    public:
        const std::string& getMessage() const noexcept
        {
            return message;
        }

#ifdef __cpp_lib_stacktrace
        const std::stacktrace& getTrace() const noexcept
        {
            return trace;
        }
#endif

    private:
        std::string message;
#ifdef __cpp_lib_stacktrace
        std::stacktrace trace;
#endif
    };

    using ExpectedSuccess = std::expected<std::true_type, Error>;
} // namespace dgm
