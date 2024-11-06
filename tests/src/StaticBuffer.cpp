#include "DGM/classes/StaticBuffer.hpp"
#include <catch2/catch_all.hpp>

namespace BufferTests
{
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

    dgm::StaticBuffer<int> createBuffer()
    {
        auto&& result = dgm::StaticBuffer<int>(3);
        result.growUnchecked() = 1;
        result.growUnchecked() = 2;
        result.growUnchecked() = 3;
        return result;
    }

    TEST_CASE("[StaticBuffer]")
    {
        SECTION("Capacity/Size", "StaticBuffer")
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

        SECTION("Remove", "StaticBuffer")
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

        SECTION("Range loop", "StaticBuffer")
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

        SECTION("const_iterator has const-qualified dereference")
        {
            auto buffer = createBuffer();
            const auto itr = std::as_const(buffer).begin();
            std::ignore = *itr;
        }

        SECTION("Smart pointers", "StaticBuffer")
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

        SECTION("constexpr", "StaticBuffer")
        {
            static_assert(constexprUsage());
        }

        SECTION("Can be moved", "StaticBuffer")
        {
            auto buffer1 = createBuffer();
            auto buffer2 = std::move(buffer1);
            REQUIRE(buffer1.getSize() == 0u);
            REQUIRE(buffer2.getSize() == 3u);
            REQUIRE(buffer2[0] == 1);
            REQUIRE(buffer2[1] == 2);
            REQUIRE(buffer2[2] == 3);
        }

        SECTION("Can be cloned", "StaticBuffer")
        {
            auto buffer1 = createBuffer();
            auto buffer2 = buffer1.clone();
            REQUIRE(buffer1.getSize() == 3u);
            REQUIRE(buffer2.getSize() == 3u);

            for (unsigned i = 0; i < buffer1.getSize(); i++)
            {
                REQUIRE(buffer1[i] == buffer2[i]);
            }
        }
    }

} // namespace BufferTests