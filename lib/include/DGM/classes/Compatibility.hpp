#pragma once

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
#define CONSTEXPR_NODISCARD constexpr [[nodiscard]]
#endif

#endif