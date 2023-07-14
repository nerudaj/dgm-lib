#include "DGM/classes/StaticBuffer.hpp"
#include <catch2/catch_all.hpp>

namespace BufferTests
{

    TEST_CASE("Capacity/Size", "StaticBuffer")
    {
        auto&& ints = dgm::StaticBuffer<int>(2);
        REQUIRE(ints.getCapacity() == 2);
        REQUIRE(ints.getSize() == 0);

        REQUIRE(ints.grow());
        REQUIRE(ints.getSize() == 1);

        REQUIRE(ints.grow());
        REQUIRE(ints.getSize() == 2);

        REQUIRE(!ints.grow());
    }

    TEST_CASE("Remove", "StaticBuffer")
    {
        auto&& ints = dgm::StaticBuffer<int>(3);
        for (unsigned i = 0; i < ints.getCapacity(); i++)
        {
            REQUIRE(ints.grow());
            ints.getLast() = i;
        }

        ints.remove(1);

        REQUIRE(ints.getSize() == 2);
        REQUIRE(ints[0] == 0);
        REQUIRE(ints[1] == 2);

        // Still exists in memory, but is not iterated over
        REQUIRE(ints[2] == 1);
    }

    TEST_CASE("Range loop", "StaticBuffer")
    {
        auto&& ints = dgm::StaticBuffer<int>(3);
        REQUIRE(ints.isEmpty());
        for (unsigned i = 0; i < ints.getCapacity(); i++)
        {
            REQUIRE(ints.grow());
            ints.getLast() = 0;
        }
        REQUIRE(ints.isFull());

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

    TEST_CASE("Smart pointers", "StaticBuffer")
    {
        SECTION("Shared pointer is stable when removing data")
        {
            auto&& ints = dgm::StaticBuffer<std::shared_ptr<int>>(3);
            ints.grow();
            ints.getLast() = std::make_shared<int>(42);
            ints.grow();
            ints.getLast() = std::make_shared<int>(69);

            int* valuePtrBackup = ints.getLast().get();
            ints.remove(ints.begin());

            REQUIRE(*(*ints.begin()) == 69);
            REQUIRE(ints.getLast().get() == valuePtrBackup);
        }
    }

    constexpr bool constexprUsage()
    {
        auto&& buffer = dgm::StaticBuffer<int>(10);
        buffer.grow();
        buffer.getLast() = 42;
        buffer.grow();
        buffer.getLast() = 24;
        buffer.remove(0);
        return buffer[0] == 24;
    }

    TEST_CASE("constexpr", "StaticBuffer")
    {
        static_assert(constexprUsage());
    }

} // namespace BufferTests