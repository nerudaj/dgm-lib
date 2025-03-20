#pragma once

#ifdef ANDROID

#ifndef NODISCARD_RESULT
#define NODISCARD_RESULT
#endif

#ifndef CONSTEXPR_NODISCARD
#define CONSTEXPR_NODISCARD constexpr
#endif

#include <vector>

namespace std
{
    namespace ranges
    {
        template<class Container, class... Args>
        Container to(Args&&... args)
        {
            return Container(std::forward<Args>(args)...);
        }
    } // namespace ranges
} // namespace std
#else
#ifndef NODISCARD_RESULT
#define NODISCARD_RESULT [[nodiscard]]
#endif

#ifndef CONSTEXPR_NODISCARD
#define CONSTEXPR_NODISCARD constexpr [[nodiscard]]
#endif

#endif