#pragma once

#include <DGM/classes/Traits.hpp>
#include <cassert>
#include <limits>
#include <variant>
#include <vector>

namespace dgm
{
    /**
     * \brief std::vector replacement with O(1) insertions and deletions
     * and stable iterators.
     *
     * \warn This class is mainly used as an underlying type for
     * dgm::SpatialBuffer. For your projects, consider using plf::colony
     * instead.
     */
    template<
        class T,
        unsigned PreallocatedMemoryAmount = 128,
        typename IndexType = std::size_t>
    class DynamicBuffer final
    {
    public:
        using DataType = T;
        using IndexingType = IndexType;

    public:
        [[nodiscard]] constexpr DynamicBuffer()
        {
            data.reserve(PreallocatedMemoryAmount);
        }

        DynamicBuffer(const DynamicBuffer&) = delete;
        [[nodiscard]] DynamicBuffer(DynamicBuffer&&) = default;
        ~DynamicBuffer() = default;

        [[nodiscard]] constexpr DynamicBuffer clone() const
        {
            return DynamicBuffer(data, firstFreeSlot);
        }

        [[nodiscard]] DynamicBuffer& operator=(DynamicBuffer&&) = default;

    public:
        class iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = ptrdiff_t;
            using value_type = T;
            using reference = value_type&;
            using pointer = value_type*;

        public:
            [[nodiscard]] constexpr iterator(
                IndexType index,
                DynamicBuffer<T, PreallocatedMemoryAmount, IndexType>&
                    backref) noexcept
                : index(index), backref(backref)
            {
                skipDeletedElements();
            }

        public:
            [[nodiscard]] constexpr std::pair<reference, IndexType>
            operator*() noexcept
            {
                return { backref[index], index };
            }

            constexpr iterator operator++() noexcept
            {
                ++index;
                skipDeletedElements();
                return *this;
            }

            constexpr iterator operator++(int) noexcept
            {
                auto copy = iterator(*this);
                ++*this;
                return copy;
            }

            [[nodiscard]] constexpr bool
            operator==(const iterator& other) const noexcept
            {
                return index == other.index;
            }

            [[nodiscard]] constexpr bool
            operator!=(const iterator& other) const noexcept
            {
                return index != other.index;
            }

        private:
            constexpr void skipDeletedElements() noexcept
            {
                while (index != backref.data.size()
                       && std::holds_alternative<Index>(backref.data[index]))
                    ++index;
            }

        private:
            IndexType index;
            DynamicBuffer<T, PreallocatedMemoryAmount, IndexType>& backref;
        };

    public:
        template<class Self>
        [[nodiscard]] constexpr auto&&
        operator[](this Self&& self, IndexType index) noexcept
        {
            return std::get<T>(self.data[index]);
        }

        template<class... Args>
        constexpr IndexType emplaceBack(Args&&... args)
        {
            if (hasNoDeletedItems())
            {
                data.emplace_back(std::forward<Args>(args)...);
                return static_cast<IndexType>(data.size() - 1);
            }
            else
            {
                auto index = firstFreeSlot;
                firstFreeSlot = std::get<Index>(data[index]).nextFreeSlot;
                data[index] = T { std::forward<Args>(args)... };
                return index;
            }
        }

        constexpr void eraseAtIndex(IndexType index) noexcept
        {
            assert(std::holds_alternative<T>(
                data[index])); // Trying to delete an already deleted item
            data[index] = Index { firstFreeSlot };
            firstFreeSlot = index;
        }

        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return iterator(0, *this);
        }

        [[nodiscard]] constexpr iterator end() noexcept
        {
            return iterator(static_cast<IndexType>(data.size()), *this);
        }

    private:
        [[nodiscard]] constexpr bool hasNoDeletedItems() const noexcept
        {
            return firstFreeSlot == std::numeric_limits<IndexType>::max();
        }

    private:
        struct Index
        {
            IndexType nextFreeSlot;

            [[nodiscard]] constexpr explicit Index(IndexType nfs) noexcept
                : nextFreeSlot(nfs)
            {
            }
        };

        using Element = std::variant<T, Index>;

        [[nodiscard]] constexpr DynamicBuffer(
            std::vector<Element> data, IndexType firstFreeSlot)
            : data(data), firstFreeSlot(firstFreeSlot)
        {
        }

        std::vector<Element> data;
        IndexType firstFreeSlot = std::numeric_limits<IndexType>::max();
    };
} // namespace dgm