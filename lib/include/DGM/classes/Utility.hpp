#pragma once

#include <DGM/classes/Traits.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <string>

namespace dgm
{

    class Utility
    {
    public:
        template<IsSfVector T>
        static std::string to_string(const T& vec)
        {
            return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y)
                   + "]";
        }

        template<IsSfVector T>
        struct less
        {
            [[nodiscard]] constexpr inline bool
            operator()(const T& a, const T& b) const noexcept
            {
                return a.y == b.y ? a.x < b.x : a.y < b.y;
            }
        };
    };
} // namespace dgm
