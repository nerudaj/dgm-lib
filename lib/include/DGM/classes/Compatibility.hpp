#pragma once

#ifdef ANDROID
#define NODISCARD_RESULT
#else
#define NODISCARD_RESULT [[nodiscard]]
#endif