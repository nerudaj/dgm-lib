#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>
#include <sstream>

#define COMPARE_UNSIGNED_VECTORS(v1, v2)                                       \
    REQUIRE(v1.x == v2.y);                                                     \
    REQUIRE(v1.y == v2.y);

TEST_CASE("Clip", "init")
{
    SECTION("Frame size explicit and within limits")
    {
        dgm::Clip clip({ 32, 32 }, { 0, 0, 256, 256 }, 12);

        REQUIRE(clip.getFrameCount() == 12u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size explicit, exceeding limits")
    {
        dgm::Clip clip({ 32, 32 }, { 0, 0, 128, 128 }, 25);

        REQUIRE(clip.getFrameCount() == 16u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size unspecified, zero frame offsets")
    {
        dgm::Clip clip({ 32, 32 }, { 0, 0, 128, 128 });

        REQUIRE(clip.getFrameCount() == 16u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size unspecified, nonzero frame offsets")
    {
        dgm::Clip clip({ 32, 32 }, { 0, 0, 128, 128 }, 0, { 24, 24 });

        REQUIRE(clip.getFrameCount() == 4u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));

        const auto& f1 = clip.getFrame(0);
        const auto& f2 = clip.getFrame(1);
        const auto& f3 = clip.getFrame(2);
        const auto& f4 = clip.getFrame(3);

        REQUIRE(f1.left == 0);
        REQUIRE(f1.top == 0);

        REQUIRE(f2.left == 56);
        REQUIRE(f2.top == 0);

        REQUIRE(f3.left == 0);
        REQUIRE(f3.top == 56);

        REQUIRE(f4.left == 56);
        REQUIRE(f4.top == 56);
    }

    SECTION(
        "Frame size unspecified, nonzero frame offset, nonzero boundaries "
        "origin")
    {
        dgm::Clip clip({ 32, 32 }, { 64, 64, 128, 128 }, 0, { 24, 24 });

        REQUIRE(clip.getFrameCount() == 4u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Just one frame")
    {
        dgm::Clip clip({ 64, 80 }, { 0, 0, 64, 80 }, 0, { 8, 8 });

        REQUIRE(clip.getFrameCount() == 1u);
    }

    SECTION("Throws if getFrame is called with out-of-bounds index")
    {
        dgm::Clip clip;

        REQUIRE_THROWS([&clip]() { std::ignore = clip.getFrame(0); }());
    }
}
