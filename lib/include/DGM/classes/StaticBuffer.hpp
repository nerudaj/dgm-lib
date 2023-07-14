#pragma once

#include <DGM/classes/Traits.hpp>
#include <cassert>

namespace dgm
{
    /**
     * \brief Dynamic buffer array with add/remove operations
     *
     * \details This is a collection in vein of std::array
     * that has O(1) addition and deletion, but order of items
     * or stability of iterators are not guaranteed after
     * removing an element.
     *
     * The template type should be default-constructible
     * and swappable. If it isn't, wrap it in a smart pointer.
     */
    template<TrivialType T>
    class StaticBuffer final
    {
    public:
        class const_iterator
        {
        public:
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef value_type& reference;
            typedef value_type* pointer;
            typedef std::random_access_iterator_tag iterator_category;

        public:
            [[nodiscard]] constexpr explicit const_iterator(T* value) noexcept
                : ptr(value)
            {
            }

            [[nodiscard]] constexpr const_iterator(const_iterator&& other) =
                default;
            [[nodiscard]] constexpr const_iterator(
                const const_iterator& other) = default;
            [[nodiscard]] constexpr ~const_iterator() = default;

        public:
            [[nodiscard]] const reference& operator*() const noexcept
            {
                return *ptr;
            }

            const_iterator& operator++() noexcept
            {
                ptr++;
                return *this;
            }

            [[nodiscard]] constexpr const_iterator operator++(int) noexcept
            {
                auto&& copy = const_iterator(*this);
                ptr++;
                return copy;
            }

            [[nodiscard]] constexpr const_iterator& operator--() noexcept
            {
                ptr--;
                return *this;
            }

            [[nodiscard]] constexpr const_iterator operator--(int) noexcept
            {
                auto&& copy = const_iterator(*this);
                ptr--;
                return copy;
            }

            [[nodiscard]] constexpr difference_type
            operator-(const const_iterator& other) const noexcept
            {
                return ptr - other.ptr;
            }

            [[nodiscard]] constexpr difference_type
            operator+(const const_iterator& other) const noexcept
            {
                return ptr + other.ptr;
            }

            [[nodiscard]] constexpr bool
            operator==(const const_iterator& second) const noexcept
            {
                return ptr == second.ptr;
            }

            [[nodiscard]] constexpr bool
            operator!=(const const_iterator& other) const noexcept
            {
                return ptr != other.ptr;
            }

            [[nodiscard]] constexpr auto
            operator<=>(const const_iterator& other) const noexcept
            {
                return ptr <=> other.ptr;
            }

        protected:
            T* ptr; ///< Pointer to data
        };

        class iterator final : public const_iterator
        {
        public:
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef value_type& reference;
            typedef value_type* pointer;
            typedef std::random_access_iterator_tag iterator_category;

        public:
            [[nodiscard]] constexpr iterator(T* value) noexcept
                : const_iterator(value)
            {
            }

            [[nodiscard]] constexpr iterator(const iterator& other) noexcept =
                default;
            [[nodiscard]] constexpr iterator(iterator&& other) noexcept =
                default;
            constexpr ~iterator() = default;

        public:
            [[nodiscard]] constexpr reference& operator*() const noexcept
            {
                return *super::ptr;
            }

            [[nodiscard]] constexpr iterator& operator++() noexcept
            {
                ++super::ptr;
                return *this;
            }

            [[nodiscard]] constexpr iterator operator++(int) noexcept
            {
                iterator copy(*this);
                ++super::ptr;
                return copy;
            }

            [[nodiscard]] constexpr iterator& operator--() noexcept
            {
                --super::ptr;
                return *this;
            }

            [[nodiscard]] constexpr iterator operator--(int) noexcept
            {
                auto&& copy = iterator(*this);
                --super::ptr;
                return copy;
            }

            [[nodiscard]] constexpr iterator&
            operator=(const iterator& other) noexcept
            {
                super::ptr = other.ptr;
                return *this;
            }

            [[nodiscard]] constexpr difference_type
            operator-(const const_iterator& other) const noexcept
            {
                return super::ptr - other.ptr;
            }

            [[nodiscard]] constexpr difference_type
            operator+(const const_iterator& other) const noexcept
            {
                return super::ptr + other.ptr;
            }

            [[nodiscard]] constexpr bool
            operator==(const iterator& second) const noexcept
            {
                return super::ptr == second.ptr;
            }

            [[nodiscard]] constexpr bool
            operator!=(const iterator& second) const noexcept
            {
                return super::ptr != second.ptr;
            }

            [[nodiscard]] constexpr auto
            operator<=>(const iterator& other) const noexcept
            {
                return super::ptr <=> other.ptr;
            }

        private:
            using super = const_iterator;
        };

    public:
        [[nodiscard]] constexpr explicit StaticBuffer(unsigned maxCapacity)
        {
            data = new T[maxCapacity];
            if (!data) throw std::bad_alloc();
            capacity = maxCapacity;
        }

        StaticBuffer& operator=(StaticBuffer other) = delete;
        StaticBuffer(const StaticBuffer& buffer) = delete;
        StaticBuffer(StaticBuffer&& buffer) = delete;

        constexpr ~StaticBuffer() noexcept
        {
            delete[] data;
            data = nullptr;
            dataSize = 0;
        }

    public:
        /**
         * \brief Add an item to buffer
         *
         * \return TRUE on success, FALSE if capacity of container has been
         * reached
         *
         * \details Container always holds N allocated and initialized items
         * where N is total capacity of container. But those items are hidden
         * until this function unhides them, making them included in range loops
         * and such.
         */
        constexpr bool grow() noexcept
        {
            if (dataSize == capacity) return false;
            return ++dataSize;
        }

        /**
         *  \brief Add item to buffer and return reference to it
         *
         *  \details Does the same thing as grow, but it always returns
         *  result of getLast even if capacity was reached.
         */
        [[nodiscard]] constexpr T& growUnchecked() noexcept
        {
            grow();
            return getLast();
        }

        /**
         * \brief Remove item at index
         *
         * \param [in] index Index of element to remove
         *
         * \details This will swap the item at position \p index
         * with the last valid item and decrease size of the container,
         * hiding the removed item.
         *
         * All iterators must be discarded after calling this function.
         */
        constexpr void remove(std::size_t index) noexcept
        {
            assert(index < dataSize);
            std::swap(data[index], data[--dataSize]);
        }

        constexpr void remove(const const_iterator& itr) noexcept
        {
            remove(itr - begin());
        }

        constexpr void remove(const iterator& itr) noexcept
        {
            remove(itr - begin());
        }

        /**
         * \brief Get element to last available item
         *
         * \details Last item equals to last added item with \ref expand
         * unless \ref remove was called. Use this immediately \ref expand
         * to initialize the unhid item.
         */
        template<class Self>
        [[nodiscard]] constexpr auto&& getLast(this Self&& self) noexcept
        {
            return self.operator[](self.dataSize - 1);
        }

        // Deducing this getter
        template<class Self>
        [[nodiscard]] constexpr auto&&
        operator[](this Self&& self, std::size_t index) noexcept
        {
            return self.data[index];
        }

        /**
         * \brief Get number of used items
         */
        [[nodiscard]] constexpr std::size_t getSize() const noexcept
        {
            return dataSize;
        }

        /**
         * \brief Get total number of available items
         */
        [[nodiscard]] constexpr std::size_t getCapacity() const noexcept
        {
            return capacity;
        }

        /**
         * \brief Test whether buffer is empty
         */
        [[nodiscard]] constexpr bool isEmpty() const noexcept
        {
            return dataSize == 0;
        }

        /**
         * \brief Test whether buffer is full
         */
        [[nodiscard]] constexpr bool isFull() const noexcept
        {
            return dataSize == capacity;
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return const_iterator(data);
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return const_iterator(data + dataSize);
        }

    protected:
        T* data = nullptr;        ///< Array of pointers to data
        std::size_t dataSize = 0; ///< Number of used items
        std::size_t capacity = 0; ///< Total number of available items
    };

    namespace traits
    {
        template<class T>
        struct is_static_buffer : std::false_type
        {
        };

        template<TrivialType T>
        struct is_static_buffer<dgm::StaticBuffer<T>> : std::true_type
        {
        };
    } // namespace traits

    template<class T>
    concept StaticBufferType = traits::is_static_buffer<T>::value;
} // namespace dgm