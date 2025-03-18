#pragma once

#ifdef ANDROID

#ifndef NODISCARD_RESULT
#define NODISCARD_RESULT
#endif

#ifndef CONSTEXPR_NODISCARD
#define CONSTEXPR_NODISCARD constexpr
#endif

namespace std
{
    namespace ranges
    {
        template<class T>
        T to(auto&& range)
        {
            return T(std::begin(range), std::end(range));
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