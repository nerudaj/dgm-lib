#include "CustomComparators.hpp"
#include <DGM/classes/TextureAtlas.hpp>
#include <catch2/catch_all.hpp>

sf::Texture createTexture(unsigned width, unsigned height)
{
    sf::Texture texture;
    texture.create(width, height);
    return texture;
}

dgm::Clip createDummyClip()
{
    return dgm::Clip(
        { 32, 32 },
        sf::IntRect {
            sf::Vector2i(10, 20),
            sf::Vector2i(84, 40),
        },
        2,
        { 12, 12 });
}

std::vector<sf::IntRect>
subdivideAreaTest(const sf::IntRect& area, const sf::Vector2i& takenTextureDims)
{
    return dgm::TextureAtlas::subdivideArea(area, takenTextureDims);
}

TEST_CASE("[TextureAtlas]")
{
    auto&& atlas = dgm::TextureAtlas(512, 512);

    SECTION("SubdivideArea")
    {
        SECTION("Returns nothing if source area is too small")
        {
            // TODO: this is not valid input
            auto&& vec = subdivideAreaTest(
                sf::IntRect(0, 0, 64, 64), sf::Vector2i(96, 96));
            REQUIRE(vec.empty());
        }

        SECTION("Only horizontal subdivision")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(128, 0, 128, 128), sf::Vector2i(96, 128));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(vec.front(), sf::IntRect(224, 0, 32, 128));
        }

        SECTION("Only vertical subdivision")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(0, 128, 128, 128), sf::Vector2i(128, 96));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(vec.front(), sf::IntRect(0, 224, 128, 32));
        }

        SECTION("Both subdivisions")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(0, 0, 128, 128), sf::Vector2i(64, 64));
            REQUIRE(2u == vec.size());
            COMPARE_INTRECTS(vec[0], sf::IntRect(64, 0, 64, 64));
            COMPARE_INTRECTS(vec[1], sf::IntRect(0, 64, 128, 64));
        }

        SECTION("Full horizontal fill")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(128, 0, 128, 128), sf::Vector2i(128, 64));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(vec.front(), sf::IntRect(128, 64, 128, 64));
        }

        SECTION("Full vertical fill")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(0, 0, 128, 128), sf::Vector2i(64, 128));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(vec.front(), sf::IntRect(64, 0, 64, 128));
        }

        SECTION("Full fill")
        {
            REQUIRE(subdivideAreaTest(
                        sf::IntRect(64, 64, 64, 64), sf::Vector2i(64, 64))
                        .empty());
        }
    }

    SECTION("Correctly adds Clip to empty texture")
    {
        auto&& texture = createTexture(128, 128);
        auto&& clip = createDummyClip();

        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        auto&& newClip = atlas.getClip(*result);

        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_INTRECTS(newClip.getFrame(0), clip.getFrame(0));
        COMPARE_INTRECTS(newClip.getFrame(1), clip.getFrame(1));
    }

    SECTION("Correctly adds second Clip (horizontal sub-area)")
    {
        auto&& texture = createTexture(128, 128);
        auto&& clip = createDummyClip();

        std::ignore = atlas.addTileset(texture, clip);

        // should be placed at [128, 0]
        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        // horizontal offset 128
        // vertical offset 0
        auto&& newClip = atlas.getClip(*result);
        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2u(138, 20), newClip.getFrame(0).getPosition());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2u(182, 20), newClip.getFrame(1).getPosition());
    }

    /*SECTION("Correctly adds second Clip (vertical sub-area)")
    {
        auto&& texture = createTexture(396, 128);
        auto&& clip = createDummyClip();

        std::ignore = atlas.addTileset(texture, clip);
        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        auto&& newClip = atlas.getClip(*result);
        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_INTRECTS(newClip.getFrame(0), clip.getFrame(0));
        COMPARE_INTRECTS(newClip.getFrame(1), clip.getFrame(1));
    }*/
}