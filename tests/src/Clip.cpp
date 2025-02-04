#include "CustomComparators.hpp"
#include <DGM/dgm.hpp>
#include <catch2/catch_all.hpp>
#include <sstream>

sf::IntRect get128x128Rect()
{
    return sf::IntRect {
        sf::Vector2i {
            0,
            0,
        },
        sf::Vector2i {
            128,
            128,
        },
    };
}

sf::IntRect get256x256Rect()
{
    return sf::IntRect {
        sf::Vector2i {
            0,
            0,
        },
        sf::Vector2i {
            256,
            256,
        },
    };
}

TEST_CASE("Clip", "init")
{
    SECTION("Frame size explicit and within limits")
    {
        dgm::Clip clip({ 32, 32 }, get256x256Rect(), 12);

        REQUIRE(clip.getFrameCount() == 12u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size explicit, exceeding limits")
    {
        dgm::Clip clip({ 32, 32 }, get128x128Rect(), 25);

        REQUIRE(clip.getFrameCount() == 16u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size unspecified, zero frame offsets")
    {
        dgm::Clip clip({ 32, 32 }, get128x128Rect());

        REQUIRE(clip.getFrameCount() == 16u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Frame size unspecified, nonzero frame offsets")
    {
        dgm::Clip clip({ 32, 32 }, get128x128Rect(), 0, { 24, 24 });

        REQUIRE(clip.getFrameCount() == 4u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));

        const auto& f1 = clip.getFrame(0);
        const auto& f2 = clip.getFrame(1);
        const auto& f3 = clip.getFrame(2);
        const auto& f4 = clip.getFrame(3);

        COMPARE_SIGNED_VECTORS(f1.position, sf::Vector2i(0, 0));
        COMPARE_SIGNED_VECTORS(f2.position, sf::Vector2i(56, 0));
        COMPARE_SIGNED_VECTORS(f3.position, sf::Vector2i(0, 56));
        COMPARE_SIGNED_VECTORS(f4.position, sf::Vector2i(56, 56));
    }

    SECTION(
        "Frame size unspecified, nonzero frame offset, nonzero boundaries "
        "origin")
    {
        dgm::Clip clip(
            { 32, 32 },
            sf::IntRect { sf::Vector2i(64, 64), sf::Vector2i(128, 128) },
            0,
            { 24, 24 });

        REQUIRE(clip.getFrameCount() == 4u);
        COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(32u, 32u));
    }

    SECTION("Just one frame")
    {
        dgm::Clip clip(
            { 64, 80 },
            sf::IntRect { sf::Vector2i(0, 0), sf::Vector2i(64, 80) },
            0,
            { 8, 8 });

        REQUIRE(clip.getFrameCount() == 1u);
    }

    SECTION("Throws if getFrame is called with out-of-bounds index")
    {
        auto&& clip = dgm::Clip(
            { 1, 1 }, sf::IntRect { sf::Vector2i(0, 0), sf::Vector2i(1, 1) });

        REQUIRE_THROWS([&clip]() { std::ignore = clip.getFrame(1); }());
    }
}
