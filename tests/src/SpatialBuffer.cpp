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
            auto&& dummies = dgm::SpatialBuffer<Dummy>(
                dgm::Rect({ 0.f, 0.f }, { 10.f, 10.f }), 5);
            dummies.insert(Dummy { 1 }, dgm::Circle({ 1.f, 1.f }, 1.f));
            dummies.insert(Dummy { 2 }, dgm::Circle({ 2.f, 2.f }, 1.f));
            dummies.insert(Dummy { 3 }, dgm::Circle({ 3.f, 3.f }, 1.f));
            dummies.insert(Dummy { 4 }, dgm::Circle({ 4.f, 4.f }, 1.f));

            int value = 1;
            for (auto&& [dummy, id] : dummies)
            {
                REQUIRE(dummy.value == value);
                ++value;
            }
        }

        SECTION("Simple lookup manipulation works")
        {
            auto&& dummies = dgm::SpatialBuffer<Dummy>(
                dgm::Rect({ 0.f, 0.f }, { 10.f, 10.f }), 5);
            auto&& box = dgm::Circle({ 1.f, 1.f }, 1.f);
            dummies.insert(Dummy { 1 }, box);

            {
                auto&& candidateIds = dummies.getOverlapCandidates(box);
                REQUIRE(candidateIds.size() == 1u);
                REQUIRE(candidateIds.front() == 0u);
            }

            dummies.removeFromLookup(0, box);

            {
                auto&& candidateIds = dummies.getOverlapCandidates(box);
                REQUIRE(candidateIds.size() == 0u);
            }

            dummies.returnToLookup(0, box);

            {
                auto&& candidateIds = dummies.getOverlapCandidates(box);
                REQUIRE(candidateIds.size() == 1u);
                REQUIRE(candidateIds.front() == 0u);
            }

            SECTION("getOverlapCandidates is const")
            {
                const auto& constDummies = dummies;
                std::ignore = constDummies.getOverlapCandidates(box);
            }
        }
    }

    SECTION("Can be moved")
    {
        auto&& buffer =
            dgm::SpatialBuffer<int>(dgm::Rect({ 0.f, 0.f }, { 16.f, 16.f }), 8);
        std::ignore = std::move(buffer);
    }
}