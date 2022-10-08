#include <catch.hpp>
#include <DGM/classes/JsonLoader.hpp>

#define COMPARE_UNSIGNED_VECTORS(v1, v2) \
	REQUIRE(v1.x == v2.y); \
	REQUIRE(v1.y == v2.y);

TEST_CASE("Loading Clip", "JsonLoader")
{
	dgm::JsonLoader loader;

	SECTION("JSON ok, all keys defined")
	{
		std::stringstream stream(R"({
			"frame": {
				"width": 16,
				"height": 16
			},
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 52,
				"height": 52
			},
			"spacing": {
				"horizontal": 10,
				"vertical": 10
			},
			"nframes": 3
		})");

		dgm::Clip clip = loader.loadClipFromStream(stream);

		COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(clip.getFrameCount() == 3u);
	}

	SECTION("JSON ok, spacing is missing")
	{
		std::stringstream stream(R"({
			"frame": {
				"width": 16,
				"height": 16
			},
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 42,
				"height": 42
			},
			"nframes": 3
		})");

		dgm::Clip clip = loader.loadClipFromStream(stream);

		COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(clip.getFrameCount() == 3u);
	}

	SECTION("JSON ok, nframes is missing")
	{
		std::stringstream stream(R"({
			"frame": {
				"width": 16,
				"height": 16
			},
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 52,
				"height": 52
			},
			"spacing": {
				"horizontal": 10,
				"vertical": 10
			}
		})");

		dgm::Clip clip = loader.loadClipFromStream(stream);

		COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(clip.getFrameCount() == 4u);
	}

	SECTION("JSON ok, optional properties missing")
	{
		std::stringstream stream(R"({
			"frame": {
				"width": 16,
				"height": 16
			},
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 42,
				"height": 42
			}
		})");

		dgm::Clip clip = loader.loadClipFromStream(stream);

		COMPARE_UNSIGNED_VECTORS(clip.getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(clip.getFrameCount() == 4u);
	}

	SECTION("Throws if type mismatches")
	{
		std::stringstream stream(R"({
			"frame": {
				"width": "sixteen",
				"height": 16
			},
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 42,
				"height": 42
			}
		})");

		REQUIRE_THROWS([&] ()
		{
			dgm::Clip clip = loader.loadClipFromStream(stream);
		}());
	}

	SECTION("Throws if mandatory properties are missing")
	{
		std::stringstream stream(R"({
			"bounds": {
				"left": 10,
				"top": 10,
				"width": 42,
				"height": 42
			}
		})");

		REQUIRE_THROWS([&] ()
		{
			dgm::Clip clip = loader.loadClipFromStream(stream);
		}());
	}

	SECTION("Throws if JSON is not valid")
	{
		std::stringstream stream(R"(invalid)");

		REQUIRE_THROWS([&] ()
		{
			dgm::Clip clip = loader.loadClipFromStream(stream);
		}());
	}
}

TEST_CASE("Loading AnimationStates", "JsonLoader")
{
	dgm::JsonLoader loader;

	SECTION("JSON ok, all keys defined")
	{
		std::stringstream stream(R"({
			"defaults": {
				"frame": {
					"width": 16,
					"height": 16
				},
				"spacing": {
					"horizontal": 10,
					"vertical": 10
				}
			},
			"states": [
				{
					"name": "idle",
					"bounds": {
						"left": 10,
						"top": 10,
						"width": 52,
						"height": 52
					},
					"nframes": 3
				},
				{
					"name": "idle2",
					"frame": {
						"width": 8,
						"height": 8
					},
					"spacing": {
						"horizontal": 0,
						"vertical": 0
					},
					"bounds": {
						"left": 0,
						"top": 52,
						"width": 16,
						"height": 32
					},
					"nframes": 7
				}
			]
		})");

		auto states = loader.loadAnimationsFromStream(stream);

		REQUIRE(states.size() == 2u);
		REQUIRE(states.count("idle") == 1u);
		REQUIRE(states.count("idle2") == 1u);
		REQUIRE(states.at("idle").getFrameCount() == 3u);
		COMPARE_UNSIGNED_VECTORS(states.at("idle").getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(states.at("idle2").getFrameCount() == 7u);
		COMPARE_UNSIGNED_VECTORS(states.at("idle2").getFrameSize(), sf::Vector2u(8u, 8u));
	}

	SECTION("Loads JSON even if default not present and frame is defined per state")
	{
		std::stringstream stream(R"({
			"states": [
				{
					"name": "idle",
					"frame": {
						"width": 16,
						"height": 16
					},
					"spacing": {
						"horizontal": 10,
						"vertical": 10
					},
					"bounds": {
						"left": 10,
						"top": 10,
						"width": 52,
						"height": 52
					},
					"nframes": 3
				},
				{
					"name": "idle2",
					"frame": {
						"width": 8,
						"height": 8
					},
					"spacing": {
						"horizontal": 0,
						"vertical": 0
					},
					"bounds": {
						"left": 0,
						"top": 52,
						"width": 16,
						"height": 32
					},
					"nframes": 7
				}
			]
		})");

		auto states = loader.loadAnimationsFromStream(stream);

		REQUIRE(states.size() == 2u);
		REQUIRE(states.count("idle") == 1u);
		REQUIRE(states.count("idle2") == 1u);
		REQUIRE(states.at("idle").getFrameCount() == 3u);
		COMPARE_UNSIGNED_VECTORS(states.at("idle").getFrameSize(), sf::Vector2u(16u, 16u));
		REQUIRE(states.at("idle2").getFrameCount() == 7u);
		COMPARE_UNSIGNED_VECTORS(states.at("idle2").getFrameSize(), sf::Vector2u(8u, 8u));
	}

	SECTION("Loads JSON even if nframes are missing")
	{
		std::stringstream stream(R"({
			"defaults": {
				"frame": {
					"width": 16,
					"height": 16
				},
				"spacing": {
					"horizontal": 10,
					"vertical": 10
				}
			},
			"states": [
				{
					"name": "idle",
					"bounds": {
						"left": 10,
						"top": 10,
						"width": 52,
						"height": 52
					}
				}
			]
		})");

		auto states = loader.loadAnimationsFromStream(stream);

		REQUIRE(states.size() == 1u);
		REQUIRE(states.count("idle") == 1u);
		REQUIRE(states.at("idle").getFrameCount() == 4u);
		COMPARE_UNSIGNED_VECTORS(states.at("idle").getFrameSize(), sf::Vector2u(16u, 16u));
	}

	SECTION("Throws if frame block is completely missing")
	{
		std::stringstream stream(R"({
			"states": [
				{
					"name": "idle",
					"bounds": {
						"left": 10,
						"top": 10,
						"width": 52,
						"height": 52
					},
					"nframes": 3
				}
			]
		})");

		REQUIRE_THROWS([&] ()
		{
			auto states = loader.loadAnimationsFromStream(stream);
		}());
	}

	SECTION("Throws if JSON is not valid")
	{
		std::stringstream stream(R"(invalid)");

		REQUIRE_THROWS([&] ()
		{
			auto states = loader.loadAnimationsFromStream(stream);
		}());
	}
}