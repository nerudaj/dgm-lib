#pragma once

#include <version>

#ifndef __cpp_lib_ranges_enumerate
#include <ranges>
#include <vector>

namespace std
{
    namespace ranges
    {
        namespace views
        {
            auto enumerate(auto&& r)
            {
                return std::views::zip(
                    std::views::iota(0, static_cast<ptrdiff_t>(r.size())), r);
            }
        } // namespace views
    } // namespace ranges
} // namespace std
#endif

#ifdef ANDROID

#ifndef NODISCARD_RESULT
#define NODISCARD_RESULT
#endif

#ifndef CONSTEXPR_NODISCARD
#define CONSTEXPR_NODISCARD constexpr
#endif

#else
#ifndef NODISCARD_RESULT
#define NODISCARD_RESULT [[nodiscard]]
#endif

#ifndef CONSTEXPR_NODISCARD
#define CONSTEXPR_NODISCARD [[nodiscard]] constexpr
#endif

#endif