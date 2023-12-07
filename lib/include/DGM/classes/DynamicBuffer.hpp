#pragma once

#include <DGM/classes/Traits.hpp>
#include <cassert>
#include <limits>
#include <optional>
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
    class [[nodiscard]] DynamicBuffer final
    {
    public:
        using DataType = T;
        using IndexingType = IndexType;

    public:
        constexpr DynamicBuffer()
        {
            data.reserve(PreallocatedMemoryAmount);
        }

        DynamicBuffer(const DynamicBuffer&) = delete;
        DynamicBuffer(DynamicBuffer&&) = default;
        ~DynamicBuffer() = default;

        [[nodiscard]] constexpr DynamicBuffer clone() const
        {
            return DynamicBuffer(data, firstFreeSlot);
        }

        DynamicBuffer& operator=(DynamicBuffer&&) = default;

    public:
        template<
            class BackrefType,
            bool IsConst =
                std::is_const_v<std::remove_reference_t<BackrefType>>>
            requires std::is_reference_v<BackrefType>
        class [[nodiscard]] IteratorBase final
        {
        public:
            typedef std::random_access_iterator_tag iterator_category;

        public:
            constexpr IteratorBase(
                IndexType index, BackrefType backref) noexcept
                : index(index), backref(backref)
            {
                skipDeletedElements();
            }

        public:
            [[nodiscard]] std::pair<
                std::conditional_t<IsConst, const DataType&, DataType&>,
                IndexType>
            operator*() noexcept
            {
                return { backref[index], index };
            }

            constexpr [[nodiscard]] IteratorBase<BackrefType>&
            operator++() noexcept
            {
                ++index;
                skipDeletedElements();
                return *this;
            }

            constexpr [[nodiscard]] IteratorBase<BackrefType>
            operator++(int) noexcept
            {
                auto copy = IteratorBase<BackrefType>(*this);
                ++*this;
                return copy;
            }

            [[nodiscard]] constexpr bool
            operator==(const IteratorBase<BackrefType>& other) const noexcept
            {
                return index == other.index;
            }

            [[nodiscard]] constexpr bool
            operator!=(const IteratorBase<BackrefType>& other) const noexcept
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
            BackrefType backref;
        };

        using SelfType = DynamicBuffer<T, PreallocatedMemoryAmount, IndexType>;
        using iterator = IteratorBase<SelfType&>;
        using const_iterator = IteratorBase<const SelfType&>;

    public:
        [[nodiscard]] constexpr bool
        isIndexValid(IndexType index) const noexcept
        {
            return index < data.size()
                   && std::holds_alternative<T>(data[index]);
        }

        template<class Self>
        [[nodiscard]] constexpr auto&&
        operator[](this Self&& self, IndexType index) noexcept
        {
            return std::get<T>(self.data[index]);
        }

        [[nodiscard]] constexpr std::optional<std::reference_wrapper<T>>
        at(IndexType index) noexcept
        {
            try
            {
                return std::get<T>(data[index]);
            }
            catch (...)
            {
                return std::nullopt;
            }
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
            assert(isIndexValid(
                index)); // Trying to delete an already deleted item
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

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return const_iterator(0, std::cref(*this));
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return const_iterator(
                static_cast<IndexType>(data.size()), std::cref(*this));
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