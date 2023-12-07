#include <DGM/classes/DynamicBuffer.hpp>
#include <catch2/catch_all.hpp>

struct Dummy
{
    int value;

    Dummy(int value) : value(value) {}
};

TEST_CASE("[DynamicBuffer]")
{
    SECTION("Iterators")
    {
        SECTION("Basic iteration works")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(2);
            buffer.emplaceBack(3);

            int value = 1;
            for (auto&& [dummy, _] : buffer)
            {
                REQUIRE(value == dummy.value);
                ++value;
            }
        }

        SECTION("const buffer can be iterated with range-based for")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(2);
            buffer.emplaceBack(3);
            const auto copy = buffer.clone();

            int value = 1;
            for (auto&& [dummy, _] : copy)
            {
                REQUIRE(value == dummy.value);
                ++value;
            }
        }

        SECTION("Iterator correctly skips elements at the start")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(2);
            buffer.eraseAtIndex(0);

            int value = 2;
            for (auto&& [dummy, _] : buffer)
            {
                REQUIRE(value == dummy.value);
                ++value;
            }
        }

        SECTION("Iterator skips deleted elements in the middle")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(24);
            buffer.emplaceBack(69);
            buffer.emplaceBack(2);

            buffer.eraseAtIndex(1);
            buffer.eraseAtIndex(2);

            int value = 1;
            for (auto&& [dummy, _] : buffer)
            {
                REQUIRE(value == dummy.value);
                ++value;
            }
        }

        SECTION("Iterator correctly skips deleted elements at the end")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(2);
            buffer.emplaceBack(24);
            buffer.eraseAtIndex(2);

            int value = 1;
            for (auto&& [dummy, _] : buffer)
            {
                REQUIRE(value == dummy.value);
                ++value;
            }
        }

        SECTION("Iterated item can be changed")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);

            for (auto&& [dummy, _] : buffer)
                dummy.value = 42;

            REQUIRE(buffer[0].value == 42);
        }
    }

    SECTION("Basic workflow")
    {
        SECTION("Can insert-delete-insert")
        {
            dgm::DynamicBuffer<Dummy> buffer;
            buffer.emplaceBack(1);
            buffer.emplaceBack(2);
            buffer.eraseAtIndex(0);
            buffer.emplaceBack(3);

            int value = 3;
            for (auto&& [dummy, _] : buffer)
            {
                REQUIRE(value == dummy.value);
                --value;
            }
        }

        SECTION("Can insert std::unique_ptr")
        {
            dgm::DynamicBuffer<std::unique_ptr<Dummy>> dummies;
            dummies.emplaceBack(std::make_unique<Dummy>(1));
        }
    }

    SECTION("decltype")
    {
        dgm::DynamicBuffer<Dummy, 64, std::uint8_t> buffer;
        static_assert(std::is_same_v<Dummy, decltype(buffer)::DataType>);
        static_assert(
            std::is_same_v<std::uint8_t, decltype(buffer)::IndexingType>);
    }

    SECTION("Can test existence of an item")
    {
        dgm::DynamicBuffer<Dummy> buffer;
        buffer.emplaceBack(Dummy { 1 });
        buffer.emplaceBack(Dummy { 2 });
        buffer.eraseAtIndex(0);
        REQUIRE_FALSE(buffer.isIndexValid(0));
        REQUIRE_FALSE(
            buffer.isIndexValid(std::numeric_limits<std::size_t>::max()));
    }

    SECTION("Can get reference to item safely with at()")
    {
        dgm::DynamicBuffer<Dummy> buffer;
        buffer.emplaceBack(Dummy { 1 });
        buffer.emplaceBack(Dummy { 2 });
        buffer.eraseAtIndex(0);
        REQUIRE_FALSE(buffer.at(0).has_value());
        REQUIRE(buffer.at(1).has_value());

        buffer.at(1).value().get().value = 10;
        REQUIRE(buffer[1].value == 10);
    }
}