#include <catch.hpp>
#include <DGM/classes/ResourceManager.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include "TestDataDir.hpp"

class LoaderMock : public dgm::LoaderInterface
{
public:
	[[nodiscard]]
	virtual dgm::Clip loadClipFromFile(const std::string&) const override
	{
		return dgm::Clip();
	}

	[[nodiscard]]
	virtual dgm::Clip loadClipFromStream(std::istream&) const override
	{
		return dgm::Clip();
	}

	[[nodiscard]]
	virtual std::shared_ptr<dgm::AnimationStates> loadAnimationsFromFile(const std::string& filename) const override
	{
		return std::make_shared<dgm::AnimationStates>();
	}

	[[nodiscard]]
	virtual std::shared_ptr<dgm::AnimationStates> loadAnimationsFromStream(std::istream&) const override
	{
		return std::make_shared<dgm::AnimationStates>();
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
			resmgr.loadResourceDir<std::shared_ptr<dgm::AnimationStates>>(
				TEST_DATA_DIR,
				{ ".json" },
				false);

			REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
			REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
			REQUIRE(!resmgr.isResourceInDatabase("statesB.json"));
		}

		SECTION("Recursive")
		{
			resmgr.loadResourceDir<std::shared_ptr<dgm::AnimationStates>>(
				TEST_DATA_DIR,
				{ ".json" },
				true);

			REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
			REQUIRE(!resmgr.isResourceInDatabase("to_be_skipped.txt"));
			REQUIRE(resmgr.isResourceInDatabase("statesB.json"));
		}
	}

	SECTION("Can deal with allowedExtensions without dots")
	{
		resmgr.loadResourceDir<std::shared_ptr<dgm::AnimationStates>>(
				TEST_DATA_DIR,
				{ "json" },
				false);

		REQUIRE(resmgr.isResourceInDatabase("statesA.json"));
		REQUIRE(!resmgr.isResourceInDatabase("statesB.json"));
	}

	SECTION("Throws if allowedExtensions are empty")
	{
		REQUIRE_THROWS([&] ()
		{
			resmgr.loadResourceDir<sf::SoundBuffer>(
				"nonexistent",
				{});
		}());
	}

	SECTION("Throws if resource directory does not exist")
	{
		REQUIRE_THROWS([&] ()
		{
			resmgr.loadResourceDir<sf::Texture>(
				"nonexistent",
	{ ".png" });
		}());
	}

	SECTION("Throws if resource does not exist")
	{
		REQUIRE_THROWS([&] ()
		{
			resmgr.loadResource<sf::Font>(
				"nonexistent.ttf");
		}());
	}
}