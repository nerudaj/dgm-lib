#pragma once

#include <cassert>
#include "Traits.hpp"

namespace dgm {
	/**
	* \brief Dynamic buffer array with add/remove operations
	*
	* \details This class is useful for cases where you need
	* constant-sized memory block (dynamically allocated),
	* which has O(1) for addition/deletion of items and where
	* order of items doesn't matter.
	*
	* In addition, constructor of each item is called only
	* once when the container is allocated. This way, you
	* always have to manually initialize data each time you
	* get a reference to "new" item, but you can elide
	* initialization which would remain constant anyways and
	* take a lot of time. (An example: if you have array of
	* enemies and all enemies use the same spritesheet. Even
	* though you always have to set their initial lives, position
	* and such, you don't have to load the texture each time,
	* saving you a lot of time).
	*
	* Also, if you obtain pointer to stored item once, this
	* pointer is guaranteed to be valid until the container
	* destructor is called.
	*/
	template<typename T, typename A = std::allocator<T>, bool SmartPtrUsed = IsSmartPtr<T>, class PointerType = std::conditional<SmartPtrUsed, T, T*>::type >
	class Buffer {
	protected:
		PointerType* data = nullptr; ///< Array of pointers to data
		std::size_t dataSize = 0; ///< Number of used items
		std::size_t dataCapacity = 0; ///< Number of allocated items

	public:
		class const_iterator {
		protected:
			PointerType *ptr; ///< Pointer to data

		public:
			typedef ptrdiff_t difference_type;
			typedef T value_type;
			typedef value_type& reference;
			typedef value_type* pointer;
			typedef std::random_access_iterator_tag iterator_category;

			[[nodiscard]] const reference &operator*() const noexcept {
				if constexpr (SmartPtrUsed)
					return *ptr;
				else
					return **ptr;
			}

			const_iterator &operator++() noexcept {
				ptr++;
				return *this;
			}

			const_iterator operator++(int) noexcept {
				const_iterator copy(*this);
				ptr++;
				return copy;
			}

			const_iterator &operator--() noexcept {
				ptr--;
				return *this;
			}

			const_iterator operator--(int) noexcept {
				const_iterator copy(*this);
				ptr--;
				return copy;
			}

			const_iterator &operator=(const const_iterator &other) noexcept {
				ptr = other.ptr;
				return *this;
			}

			[[nodiscard]] difference_type operator-(const const_iterator &other) const noexcept {
				return ptr - other.ptr;
			}

			[[nodiscard]] difference_type operator+(const const_iterator &other) const noexcept {
				return ptr + other.ptr;
			}

			[[nodiscard]] bool operator==(const const_iterator& second) const noexcept {
				return ptr == second.ptr;
			}

			[[nodiscard]] bool operator!=(const const_iterator &other) const noexcept {
				return ptr != other.ptr;
			}

			[[nodiscard]] auto operator<=>(const const_iterator& other) const noexcept { 
				return ptr <=> other.ptr;
			}

			friend void swap(const_iterator &first, const_iterator &second) noexcept {
				std::swap(first.ptr, second.ptr);
			}

			const_iterator(PointerType* value) { ptr = value; }
			const_iterator(const_iterator &&other) { ptr = other.ptr; }
			const_iterator(const const_iterator &other) { ptr = other.ptr; }
			~const_iterator() {}
		};

		class iterator : public const_iterator {
		protected:
			PointerType *ptr; ///< Pointer to data

		public:
			typedef ptrdiff_t difference_type;
			typedef T value_type;
			typedef value_type& reference;
			typedef value_type* pointer;
			typedef std::random_access_iterator_tag iterator_category;

			[[nodiscard]] reference &operator*() const noexcept {
				if constexpr (SmartPtrUsed)
					return *ptr;
				else
					return **ptr;
			}

			iterator &operator++() noexcept {
				++ptr;
				return *this;
			}

			iterator operator++(int) noexcept {
				iterator copy(*this);
				++ptr;
				return copy;
			}

			iterator &operator--() noexcept {
				--ptr;
				return *this;
			}

			iterator operator--(int) noexcept {
				iterator copy(*this);
				--ptr;
				return copy;
			}

			[[nodiscard]] iterator &operator=(const iterator &other) noexcept {
				ptr = other.ptr;
				return *this;
			}

			[[nodiscard]] difference_type operator-(const const_iterator &other) const noexcept {
				return ptr - other.ptr;
			}

			[[nodiscard]] difference_type operator+(const const_iterator &other) const noexcept {
				return ptr + other.ptr;
			}

			[[nodiscard]] bool operator==(const iterator& second) const noexcept {
				return ptr == second.ptr;
			}

			[[nodiscard]] bool operator!=(const iterator &second) const noexcept {
				return ptr != second.ptr;
			}

			[[nodiscard]] auto operator<=>(const iterator& other) const noexcept {
				return ptr <=> other.ptr;
			}

			friend void swap(iterator &first, iterator &second) noexcept {
				std::swap(first.ptr, second.ptr);
			}

			iterator(PointerType* value) { ptr = value; }
			iterator(const iterator &other) { ptr = other.ptr; }
			iterator(iterator &&other) { ptr = other.ptr; }
			~iterator() {}
		};

		/**
		* \brief Add an item to buffer
		*
		* \return TRUE on success, FALSE if capacity of container has been reached
		*
		* \details Container always holds N allocated and initialized items where
		* N is total capacity of container. But those items are hidden until
		* this function unhides them, making them included in range loops and such.
		*/
		bool expand() noexcept {
			if (dataSize == dataCapacity)
				return false;
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
		void remove(std::size_t index) noexcept {
			assert(index < dataSize);
			std::swap(data[index], data[--dataSize]);
		}

		void remove(const iterator &itr) noexcept {
			remove(itr - begin());
		}

		void remove(const const_iterator &itr) noexcept {
			remove(itr - begin());
		}

		/**
		* \brief Get element to last available item
		*
		* \details Last item equals to last added item with \ref expand
		* unless \ref remove was called. Use this immediately \ref expand
		* to initialize the unhid item.
		*/
		[[nodiscard]] T &last() noexcept {
			return this->operator[](dataSize - 1);
		}

		/**
		* \brief Get element to last available item
		*
		* \details Last item equals to last added item with \ref expand
		* unless \ref remove was called. Use this immediately \ref expand
		* to initialize the unhid item.
		*/
		[[nodiscard]] const T &last() const noexcept {
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
		void resize(std::size_t maxSize) {
			const std::size_t unallocatedSectionStart = data ? dataCapacity : 0;

			// Upscaling buffer
			if (data) {
				// Allocate bigger array
				PointerType *newData = new PointerType[maxSize];
				if (!newData)
					throw std::bad_alloc();

				// Copy valid pointers
				for (size_t i = 0; i < dataCapacity; i++)
					std::swap(newData[i], data[i]);

				// Free old array, assign new array
				delete[] data;
				data = newData;
			}
			// Creating buffer
			else {
				// Allocate bigger array
				data = new PointerType[maxSize];
				if (!data)
					throw std::bad_alloc();
			}

			// Allocate pointers (only relevant when smart pointers are not involved)
			if constexpr (!SmartPtrUsed) {
				for (std::size_t i = unallocatedSectionStart; i < maxSize; i++) {
					data[i] = new T;
					if (!data[i])
						throw std::bad_alloc();
				}
			}

			dataCapacity = maxSize;
		}

		[[nodiscard]] T &operator[] (std::size_t index) {
			if constexpr (SmartPtrUsed)
				return data[index];
			else
				return *data[index];
		}

		[[nodiscard]] const T &operator[] (std::size_t index) const {
			if constexpr (SmartPtrUsed)
				return data[index];
			else
				return *data[index];
		}

		/**
		* \brief Get number of used items
		*/
		[[nodiscard]] constexpr std::size_t size() const noexcept { return dataSize; }

		/**
		* \brief Get total number of available items
		*/
		[[nodiscard]] constexpr std::size_t capacity() const noexcept { return dataCapacity; }

		/**
		* \brief Test whether buffer is empty
		*/
		[[nodiscard]] constexpr bool empty() const noexcept { return dataSize == 0; }

		/**
		* \brief Test whether buffer is full
		*/
		[[nodiscard]] constexpr bool full() const noexcept { return dataSize == dataCapacity; }

		[[nodiscard]] const_iterator begin() const noexcept { return const_iterator(data); }

		[[nodiscard]] const_iterator end() const noexcept { return const_iterator(data + dataSize); }

		Buffer &operator=(Buffer other) = delete;

		Buffer() = default;
		Buffer(std::size_t maxSize) { resize(maxSize); }
		Buffer(const Buffer &buffer) = delete;
		Buffer(Buffer &&buffer) = delete;

		~Buffer() {
			if (data) {
				// Free each item pointer  (only relevant when smart pointers are not involved)
				if constexpr (!SmartPtrUsed) {
					for (unsigned i = 0; i < dataCapacity; i++)
						delete data[i];
				}

				delete[] data;
			}

			data = nullptr;
			dataSize = 0;
			dataCapacity = 0;
		}
	};
}