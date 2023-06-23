#include "DGM/classes/Buffer.hpp"
#include <catch2/catch_all.hpp>

namespace BufferTests
{

    TEST_CASE("Capacity/Size", "Buffer")
    {
        dgm::Buffer<int> ints(2);
        REQUIRE(ints.capacity() == 2);
        REQUIRE(ints.size() == 0);

        REQUIRE(ints.expand());
        REQUIRE(ints.size() == 1);

        REQUIRE(ints.expand());
        REQUIRE(ints.size() == 2);

        REQUIRE(!ints.expand());
    }

    TEST_CASE("Late init", "Buffer")
    {
        dgm::Buffer<int> ints;
        REQUIRE_NOTHROW(ints.resize(3));
    }

    TEST_CASE("Late resize", "Buffer")
    {
        dgm::Buffer<int> ints(3);
        for (unsigned i = 0; i < ints.capacity(); i++)
        {
            REQUIRE(ints.expand());
            ints.last() = i;
        }

        REQUIRE_NOTHROW(ints.resize(6));
        REQUIRE(ints.size() == 3);
        REQUIRE(ints.capacity() == 6);
        REQUIRE(ints[0] == 0);
        REQUIRE(ints[1] == 1);
        REQUIRE(ints[2] == 2);
    }

    TEST_CASE("Remove", "Buffer")
    {
        dgm::Buffer<int> ints(3);
        for (unsigned i = 0; i < ints.capacity(); i++)
        {
            REQUIRE(ints.expand());
            ints.last() = i;
        }

        ints.remove(1);

        REQUIRE(ints.size() == 2);
        REQUIRE(ints[0] == 0);
        REQUIRE(ints[1] == 2);

        // Still exists in memory, but is not iterated over
        REQUIRE(ints[2] == 1);
    }

    TEST_CASE("Range loop", "Buffer")
    {
        dgm::Buffer<int> ints(3);
        REQUIRE(ints.empty());
        for (unsigned i = 0; i < ints.capacity(); i++)
        {
            REQUIRE(ints.expand());
            ints.last() = 0;
        }
        REQUIRE(ints.full());

        SECTION("non-const")
        {
            unsigned cnt = 0;
            for (auto&& i : ints)
            {
                REQUIRE(i == 0);
                cnt++;
            }
            REQUIRE(cnt == 3);
        }

        SECTION("const")
        {
            unsigned cnt = 0;
            for (auto&& i : std::as_const(ints))
            {
                REQUIRE(i == 0);
                cnt++;
            }
            REQUIRE(cnt == 3);
        }
    }

    TEST_CASE("Smart pointers", "Buffer")
    {
        SECTION("Pointer is stable when resizing")
        {
            dgm::Buffer<std::unique_ptr<int>> ints(1);

            ints.expand();
            ints.last() = std::make_unique<int>(10);
            auto* ptrBackup = &(ints.last());
            int* valuePtrBackup = ints.last().get();

            // After resize, unique_ptr object should be different
            // but the managed pointer should stay the same
            ints.resize(2);
            REQUIRE(&ints[0] != ptrBackup);
            REQUIRE(ints[0].get() == valuePtrBackup);
        }

        SECTION("Pointer is stable when removing data")
        {
            dgm::Buffer<std::shared_ptr<int>> ints(2);
            ints.expand();
            ints.last() = std::make_shared<int>(42);
            ints.expand();
            ints.last() = std::make_shared<int>(69);

            int* valuePtrBackup = ints.last().get();
            ints.remove(ints.begin());

            REQUIRE(*(*ints.begin()) == 69);
            REQUIRE(ints.last().get() == valuePtrBackup);
        }
    }

    constexpr bool constexprUsage()
    {
        auto&& buffer = dgm::Buffer<int>(10);
        buffer.expand();
        buffer.last() = 42;
        buffer.expand();
        buffer.last() = 24;
        buffer.remove(0);
        return buffer[0] == 24;
    }

    TEST_CASE("constexpr", "Buffer")
    {
        static_assert(constexprUsage());
    }

    /*TEST_CASE("Algorithm support", "Buffer") {
            dgm::Buffer<int> ints(10);
            for (unsigned i = 0; i < ints.capacity(); i++) {
                    REQUIRE(ints.expand());
                    ints.last() = rand() % 256;
            }

            std::sort(ints.begin(), ints.end());

            REQUIRE(ints.size() == ints.capacity());
            for (unsigned i = 1; i < ints.size(); i++) {
                    REQUIRE(ints[i - 1] < ints[i]);
            }
    }*/

} // namespace BufferTests