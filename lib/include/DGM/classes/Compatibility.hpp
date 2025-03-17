#pragma once

#ifdef ANDROID
#include <cstdint>
#define NODISCARD_RESULT
using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using SHORT = std::int16_t;

typedef struct _XINPUT_GAMEPAD
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

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
#define NODISCARD_RESULT [[nodiscard]]
#endif