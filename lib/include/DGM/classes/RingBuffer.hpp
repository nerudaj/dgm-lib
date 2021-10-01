#pragma once

namespace dgm {
	template<typename T, unsigned SIZE>
	class RingBuffer {
	protected:
		T data[SIZE];
		unsigned start = 0;
		unsigned end = 0;
		unsigned used = 0;

		unsigned sanitizeIndex(unsigned index) const {
			return index % SIZE;
		}

	public:
		bool empty() const {
			return size() == 0;
		}

		bool full() const {
			return size() == SIZE;
		}

		unsigned size() const {
			return used;
		}

		unsigned capacity() const {
			return SIZE;
		}

		void pushBack(const T& item) {
			if (!empty()) {
				end = sanitizeIndex(end + 1);
				start = sanitizeIndex(start + 1);
			}

			data[end] = item;
			used++;
		}

		void popFront() {
			if (empty()) return;
			start = sanitizeIndex(start + 1);
			used--;
		}

		T& front() {
			return data[start];
		}

		const T& front() const {
			return data[start];
		}

		T& last() {
			return data[end];
		}
		
		const T& last() const {
			return data[end];
		}

		T& operator[](unsigned index) {
			return data[sanitizeIndex(start + index)];
		}

		const T& operator[](unsigned index) const {
			return data[sanitizeIndex(start + index)];
		}

		RingBuffer& operator=(RingBuffer other) = delete;
	};
}