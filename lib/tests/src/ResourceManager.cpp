#include "TestDataDir.hpp"
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/ResourceManager.hpp>
#include <catch.hpp>

class LoaderMock final : public dgm::LoaderInterface
{
public:
    [[nodiscard]] virtual dgm::Clip
    loadClipFromFile(const std::string&) const override
    {
        return dgm::Clip();
    }

    [[nodiscard]] dgm::Clip loadClipFromStream(std::istream&) const
    {
        return dgm::Clip();
    }

    [[nodiscard]] virtual dgm::AnimationStates
    loadAnimationsFromFile(const std::string&) const override
    {
        return dgm::AnimationStates();
    }

    [[nodiscard]] dgm::AnimationStates
    loadAnimationsFromStream(std::istream&) const
    {
        return dgm::AnimationStates();
    }
};

TEST_CASE("Can load and provide resources", "ResourceManager")
{
    LoaderMock loader;
    dgm::ResourceManager resmgr(loader);

    SECTION("Can load animation states")
    {
        SECTION("Non recursive")
        {
            resmgr.loadResourceDir<dgm::AnimationStates>(
                TEST_DATA_DIR + "/resmgr_loading", { ".json" }, false);

            REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
            REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
            REQUIRE(!resmgr.isResourceInDatabase("statesB.json"));
        }

        SECTION("Recursive")
        {
            resmgr.loadResourceDir<dgm::AnimationStates>(
                TEST_DATA_DIR + "/resmgr_loading", { ".json" }, true);

            REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
            REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
            REQUIRE(resmgr.isResourceInDatabase("statesB.json"));
        }
    }

    SECTION("Can load dgm::Clip")
    {

        SECTION("Non recursive")
        {
            resmgr.loadResourceDir<dgm::Clip>(
                TEST_DATA_DIR + "/resmgr_loading", { ".json" }, false);
            REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
            REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
            REQUIRE(!resmgr.isResourceInDatabase("statesB.json"));
        }

        SECTION("Recursive")
        {
            resmgr.loadResourceDir<dgm::Clip>(
                TEST_DATA_DIR + "/resmgr_loading", { ".json" }, true);

            REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
            REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
            REQUIRE(resmgr.isResourceInDatabase("statesB.json"));
        }
    }

    SECTION("Can deal with allowedExtensions without dots")
    {
        resmgr.loadResourceDir<dgm::AnimationStates>(
            TEST_DATA_DIR + "/resmgr_loading", { "json" }, false);

        REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
        REQUIRE(!resmgr.isResourceInDatabase("statesB.json"));
    }

    SECTION("Throws if allowedExtensions are empty")
    {
        REQUIRE_THROWS([&]() {
            resmgr.loadResourceDir<sf::SoundBuffer>("nonexistent", {});
        }());
    }

    SECTION("Throws if resource directory does not exist")
    {
        REQUIRE_THROWS([&]() {
            resmgr.loadResourceDir<sf::Texture>("nonexistent", { ".png" });
        }());
    }

    SECTION("Throws if resource does not exist")
    {
        REQUIRE_THROWS(
            [&]() { resmgr.loadResource<sf::Font>("nonexistent.ttf"); }());
    }
}