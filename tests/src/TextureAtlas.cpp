#include "CustomComparators.hpp"
#include <DGM/classes/TextureAtlas.hpp>
#include <catch2/catch_all.hpp>

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

dgm::AnimationStates createDummySpritesheet()
{
    return { {
                 "first",
                 createDummyClip(),
             },
             {
                 "second",
                 dgm::Clip(
                     { 32, 32 },
                     sf::IntRect {
                         sf::Vector2i(6, 128),
                         sf::Vector2i(96, 32),
                     }),
             } };
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
        SECTION("Only horizontal subdivision")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(sf::Vector2i(128, 0), sf::Vector2i(128, 128)),
                sf::Vector2i(96, 128));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(
                vec.front(),
                sf::IntRect(sf::Vector2i(224, 0), sf::Vector2i(32, 128)));
        }

        SECTION("Only vertical subdivision")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(sf::Vector2i(0, 128), sf::Vector2i(128, 128)),
                sf::Vector2i(128, 96));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(
                vec.front(),
                sf::IntRect(sf::Vector2i(0, 224), sf::Vector2i(128, 32)));
        }

        SECTION("Both subdivisions")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(128, 128)),
                sf::Vector2i(64, 64));
            REQUIRE(2u == vec.size());
            COMPARE_INTRECTS(
                vec[0], sf::IntRect(sf::Vector2i(64, 0), sf::Vector2i(64, 64)));
            COMPARE_INTRECTS(
                vec[1],
                sf::IntRect(sf::Vector2i(0, 64), sf::Vector2i(128, 64)));
        }

        SECTION("Full horizontal fill")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(sf::Vector2i(128, 0), sf::Vector2i(128, 128)),
                sf::Vector2i(128, 64));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(
                vec.front(),
                sf::IntRect(sf::Vector2i(128, 64), sf::Vector2i(128, 64)));
        }

        SECTION("Full vertical fill")
        {
            auto&& vec = subdivideAreaTest(
                sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(128, 128)),
                sf::Vector2i(64, 128));
            REQUIRE(1u == vec.size());
            COMPARE_INTRECTS(
                vec.front(),
                sf::IntRect(sf::Vector2i(64, 0), sf::Vector2i(64, 128)));
        }

        SECTION("Full fill")
        {
            REQUIRE(subdivideAreaTest(
                        sf::IntRect(sf::Vector2i(64, 64), sf::Vector2i(64, 64)),
                        sf::Vector2i(64, 64))
                        .empty());
        }
    }

    SECTION("Correctly adds Clip to empty texture")
    {
        auto&& texture = sf::Texture(sf::Vector2u { 128u, 128u });
        auto&& clip = createDummyClip();

        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        auto&& newClip = atlas.getClip(result.value());

        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_INTRECTS(newClip.getFrame(0), clip.getFrame(0));
        COMPARE_INTRECTS(newClip.getFrame(1), clip.getFrame(1));
    }

    SECTION("Correctly adds second Clip (horizontal sub-area)")
    {
        auto&& texture = sf::Texture(sf::Vector2u { 128u, 128u });
        auto&& clip = createDummyClip();

        std::ignore = atlas.addTileset(texture, clip);

        // should be placed at [128, 0]
        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        // horizontal offset 128
        // vertical offset 0
        auto&& newClip = atlas.getClip(result.value());
        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(138, 20), newClip.getFrame(0).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(182, 20), newClip.getFrame(1).position);
    }

    SECTION("Correctly adds second Clip (vertical sub-area)")
    {
        auto&& texture = sf::Texture(sf::Vector2u { 396u, 128u });
        auto&& clip = createDummyClip();

        std::ignore = atlas.addTileset(texture, clip);
        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        auto&& newClip = atlas.getClip(result.value());
        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(10, 148), newClip.getFrame(0).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(54, 148), newClip.getFrame(1).position);
    }

    SECTION("Correctly adds third clip")
    {
        auto&& texture = sf::Texture(sf::Vector2u { 256u, 128u });
        auto&& clip = createDummyClip();

        std::ignore = atlas.addTileset(texture, clip);
        std::ignore = atlas.addTileset(texture, clip);
        std::ignore = atlas.addTileset(texture, clip);

        // should be placed at [256, 128]
        auto&& result = atlas.addTileset(texture, clip);
        REQUIRE(result.has_value());

        auto&& newClip = atlas.getClip(result.value());
        REQUIRE(newClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(newClip.getFrameSize(), clip.getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(266, 148), newClip.getFrame(0).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(310, 148), newClip.getFrame(1).position);
    }

    SECTION("Correctly adds spritesheet")
    {
        auto&& clipTexture = sf::Texture(sf::Vector2u { 96u, 96u });
        auto&& clip = createDummyClip();

        auto&& sheetTexture = sf::Texture(sf::Vector2u { 256u, 256u });
        auto&& sheet = createDummySpritesheet();

        std::ignore = atlas.addTileset(clipTexture, clip);
        auto&& result = atlas.addSpritesheet(sheetTexture, sheet);
        REQUIRE(result.has_value());

        auto&& newSheet = atlas.getAnimationStates(result.value());
        auto&& firstClip = newSheet.at("first");
        auto&& secondClip = newSheet.at("second");

        REQUIRE(firstClip.getFrameCount() == 2u);
        COMPARE_UNSIGNED_VECTORS(
            firstClip.getFrameSize(), sheet["first"].getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(10, 116), firstClip.getFrame(0).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(54, 116), firstClip.getFrame(1).position);

        REQUIRE(secondClip.getFrameCount() == 3u);
        COMPARE_UNSIGNED_VECTORS(
            secondClip.getFrameSize(), sheet["second"].getFrameSize());
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(6, 224), secondClip.getFrame(0).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(38, 224), secondClip.getFrame(1).position);
        COMPARE_SIGNED_VECTORS(
            sf::Vector2i(70, 224), secondClip.getFrame(2).position);
    }
}