#pragma once

#include "Traits.hpp"
#include <cassert>

namespace dgm
{
    template<class T>
    concept TrivialOrSmartPointer = IsSmartPtr<T>
                                    || (std::is_default_constructible_v<T>
                                        && std::is_swappable_v<T>);

    /**
     * \brief Dynamic buffer array with add/remove operations
     *
     * \details This is a collection in vein of std::array
     * that has O(1) addition and deletion, but order of items
     * or stability of iterators are not guaranteed after
     * removing an element.
     *
     * Unlike std::array, this collection can be manually resized
     * if needed. The template type should be default-constructible
     * and swappable. If it isn't, wrap it in smart pointer.
     */
    template<
        TrivialOrSmartPointer T,
        typename Allocator = std::allocator<T>,
        bool SmartPtrUsed = IsSmartPtr<T>>
    class Buffer
    {
    protected:
        T* data = nullptr;            ///< Array of pointers to data
        std::size_t dataSize = 0;     ///< Number of used items
        std::size_t dataCapacity = 0; ///< Number of allocated items

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
        [[nodiscard]] constexpr Buffer() noexcept = default;

        [[nodiscard]] constexpr explicit Buffer(std::size_t maxSize)
        {
            resize(maxSize);
        }

        Buffer& operator=(Buffer other) = delete;
        Buffer(const Buffer& buffer) = delete;
        Buffer(Buffer&& buffer) = delete;

        constexpr ~Buffer() noexcept
        {
            delete[] data;
            data = nullptr;
            dataSize = 0;
            dataCapacity = 0;
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
        constexpr bool expand() noexcept
        {
            if (dataSize == dataCapacity) return false;
            return ++dataSize;
        }

        /**
         * \brief Remove item at index
         *
         * \param [in] index Index of element to remove
         *
         * \details This will swap the item at position \p index
         * with the last valid item and decrease size of the container,
         * hiding the removed item.
         */
        constexpr void remove(std::size_t index) noexcept
        {
            assert(index < dataSize);
            std::swap(data[index], data[--dataSize]);
        }

        constexpr void remove(const iterator& itr) noexcept
        {
            remove(itr - begin());
        }

        constexpr void remove(const const_iterator& itr) noexcept
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
        [[nodiscard]] constexpr T& last() noexcept
        {
            return this->operator[](dataSize - 1);
        }

        /**
         * \brief Get element to last available item
         *
         * \details Last item equals to last added item with \ref expand
         * unless \ref remove was called. Use this immediately \ref expand
         * to initialize the unhid item.
         */
        [[nodiscard]] constexpr const T& last() const noexcept
        {
            return this->operator[](dataSize - 1);
        }

        /**
         * \brief Resize buffer array
         *
         * \param [in] maxSize New capacity for array
         *
         * \details This method is used for setting new size of the
         * buffer array. If \p newSize is bigger than current \ref capacity
         * then new items will be constructed. If \p newSize is less, then
         * some items on high indices will be invalidated and deleted.
         *
         * If items are not deleted via this function, any pointers to them
         * are still valid, even after resize.
         *
         * \note This method is expensive. The best way is to use it once when
         * your program is initializing.
         */
        constexpr void resize(std::size_t maxSize)
        {
            // Upscaling buffer
            if (data)
            {
                // Allocate bigger array
                auto&& newData = new T[maxSize];
                if (!newData) throw std::bad_alloc();

                // Copy valid items
                for (size_t i = 0; i < dataCapacity; i++)
                    std::swap(newData[i], data[i]);

                // Free old array, assign new array
                delete[] data;
                data = newData;
            }
            // Creating buffer
            else
            {
                // Allocate bigger array
                data = new T[maxSize];
                if (!data) throw std::bad_alloc();
            }

            dataCapacity = maxSize;
        }

        /*T& operator[](std::size_t index) noexcept
        {
            return data[index];
        }

        [[nodiscard]] constexpr const T&
        operator[](std::size_t index) const noexcept
        {
            return data[index];
        }*/

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
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return dataSize;
        }

        /**
         * \brief Get total number of available items
         */
        [[nodiscard]] constexpr std::size_t capacity() const noexcept
        {
            return dataCapacity;
        }

        /**
         * \brief Test whether buffer is empty
         */
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return dataSize == 0;
        }

        /**
         * \brief Test whether buffer is full
         */
        [[nodiscard]] constexpr bool full() const noexcept
        {
            return dataSize == dataCapacity;
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return const_iterator(data);
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return const_iterator(data + dataSize);
        }
    };
} // namespace dgm