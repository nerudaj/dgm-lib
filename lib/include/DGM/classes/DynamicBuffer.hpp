#pragma once

#include <DGM/classes/Traits.hpp>
#include <limits>
#include <variant>
#include <vector>

namespace dgm
{
    template<TrivialType T>
    class DynamicBuffer final
    {
    public:
        template<class Self>
        [[nodiscard]] constexpr auto&&
        operator[](this Self&& self, std::size_t index) noexcept
        {
            return std::get<T>(self.data[index]);
        }

        template<class... Args>
        [[nodiscard]] constexpr std::size_t pushBack(Args&&... args)
        {
            if (hasNoDeletedItems())
            {
                data.emplace_back(T { std::forward<Args>(args)... });
                return data.size() - 1;
            }
            else
            {
                auto index = firstFreeSlot;
                firstFreeSlot = std::get<Index>(data[index]).nextFreeSlot;
                data[index] = T { std::forward<Args>(args)... };
                return index;
            }
        }

        void erase(std::size_t index)
        {
            data[index] = Index { firstFreeSlot };
            firstFreeSlot = index;
        }

    private:
        [[nodiscard]] constexpr bool hasNoDeletedItems() const noexcept {
            return firstFreeSlot == std::numeric_limits<std::size_t>::max();
        }

    private:
        struct Index
        {
            std::size_t nextFreeSlot;
        };

        using Element = std::variant<T, Index>;

        std::vector<Element> data;
        std::size_t firstFreeSlot = std::numeric_limits<std::size_t>::max();
    };
} // namespace dgm