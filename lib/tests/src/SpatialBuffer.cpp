#include <DGM/classes/SpatialBuffer.hpp>
#include <catch2/catch_all.hpp>

struct Dummy
{
    int value;
};

TEST_CASE("[SpatialBuffer]")
{
    SECTION("Basic usage")
    {
        SECTION("Can be iterated")
        {
            auto&& dummies = dgm::SpatialBuffer<Dummy>({ 0, 0, 10.f, 10.f }, 5);
            dummies.insert(Dummy { 1 }, dgm::Circle { 1.f, 1.f, 1.f });
            dummies.insert(Dummy { 2 }, dgm::Circle { 2.f, 2.f, 1.f });
            dummies.insert(Dummy { 3 }, dgm::Circle { 3.f, 3.f, 1.f });
            dummies.insert(Dummy { 4 }, dgm::Circle { 4.f, 4.f, 1.f });

            int value = 1;
            for (auto&& [dummy, id] : dummies)
            {
                REQUIRE(dummy.value == value);
                ++value;
            }
        }
    }
}