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
        template<
            class PtrType,
            bool IsConst = std::is_const_v<std::remove_pointer_t<PtrType>>>
        class [[nodiscard]] IteratorBase final
        {
        public:
            using ValueType = std::remove_pointer_t<PtrType>;
            using iterator_category = std::random_access_iterator_tag;

        public:
            constexpr explicit IteratorBase(PtrType ptr) noexcept : ptr(ptr)
            {
                assert(ptr);
            }

            [[nodiscard]] IteratorBase(IteratorBase<PtrType>&& other)
                : ptr(other.ptr)
            {
                other.ptr = nullptr;
            }

        public:
            [[nodiscard]] std::
                conditional_t<IsConst, const ValueType&, ValueType&>
                operator*(this std::conditional_t<
                          IsConst,
                          const IteratorBase<PtrType>&,
                          IteratorBase<PtrType>&> self) noexcept
            {
                // This function uses explicit this to be const
                // based on whether it is const_iterator or not
                return *self.ptr;
            }

            IteratorBase<PtrType>& operator++() noexcept
            {
                ptr++;
                return *this;
            }

            [[nodiscard]] constexpr IteratorBase<PtrType>
            operator++(int) noexcept
            {
                auto&& copy = IteratorBase<PtrType>(*this);
                ptr++;
                return copy;
            }

            [[nodiscard]] constexpr IteratorBase<PtrType>& operator--() noexcept
            {
                ptr--;
                return *this;
            }

            [[nodiscard]] constexpr IteratorBase<PtrType>
            operator--(int) noexcept
            {
                auto&& copy = IteratorBase<PtrType>(*this);
                ptr--;
                return copy;
            }

            [[nodiscard]] constexpr std::size_t
            operator-(const IteratorBase<PtrType>& other) const noexcept
            {
                return ptr - other.ptr;
            }

            [[nodiscard]] constexpr std::size_t
            operator+(const IteratorBase<PtrType>& other) const noexcept
            {
                return ptr + other.ptr;
            }

            [[nodiscard]] constexpr bool
            operator==(const IteratorBase<PtrType>& other) const noexcept
            {
                return ptr == other.ptr;
            }

            [[nodiscard]] constexpr bool
            operator!=(const IteratorBase<PtrType>& other) const noexcept
            {
                return ptr != other.ptr;
            }

            [[nodiscard]] constexpr auto
            operator<=>(const IteratorBase<PtrType>& other) const noexcept
            {
                return ptr <=> other.ptr;
            }

        private:
            PtrType ptr; ///< Pointer to data
        };

        using iterator = IteratorBase<T*>;
        using const_iterator = IteratorBase<const T*>;

    public:
        [[nodiscard]] constexpr explicit StaticBuffer(unsigned maxCapacity)
        {
            data = new T[maxCapacity];
            if (!data) throw std::bad_alloc();
            capacity = maxCapacity;
        }

        StaticBuffer(const StaticBuffer&) = delete;

        [[nodiscard]] constexpr StaticBuffer(StaticBuffer&& other) noexcept
        {
            std::swap(data, other.data);
            std::swap(capacity, other.capacity);
            std::swap(dataSize, other.dataSize);
        }

        constexpr ~StaticBuffer() noexcept
        {
            delete[] data;
            data = nullptr;
            dataSize = 0;
        }

        StaticBuffer& operator=(StaticBuffer&&) = default;

        /**
         *  Create a copy of buffer
         *
         *  This function runs reasonably fast if T is trivially copyable.
         *  If T is not trivially copyable then the array is iterated and
         *  the copy constructor is called per each item.
         */
        [[nodiscard]] constexpr StaticBuffer clone()
        {
            auto clonedData = new T[capacity];
            if (!clonedData) throw std::bad_alloc();

            if constexpr (std::is_trivially_copyable_v<T>)
            {
                std::memcpy(clonedData, data, sizeof(T) * capacity);
            }
            else
            {
                for (unsigned i = 0; i < capacity; i++)
                    clonedData[i] = data[i];
            }
            return StaticBuffer(clonedData, dataSize, capacity);
        }

    private:
        [[nodiscard]] constexpr StaticBuffer(
            T* data, std::size_t dataSize, std::size_t capacity) noexcept
            : data(data), dataSize(dataSize), capacity(capacity)
        {
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