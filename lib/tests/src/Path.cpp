#include <catch.hpp>
#include "DGM/classes/Path.hpp"

namespace PathTests {

	const std::vector<dgm::TileNavpoint> POINTS = {
				{{0, 0}, 10}, {{1, 0}, 0}, {{2, 0}, 5}
	};

	const LevelD::Path LVD_PATH = {
		.looping = false,
		.tag = 0,
		.points = {
			{ 123, 46, 0 },
			{ 45, 89, 10 },
			{ 1000, 2000, 30 }
		}
	};

	TEST_CASE("Construction", "Path") {
		SECTION("From vector") {
			REQUIRE_NOTHROW(dgm::Path(POINTS, true));
			REQUIRE_NOTHROW(dgm::Path(dgm::Path(POINTS, true)));
		}

		SECTION("From LevelD") {
			REQUIRE_NOTHROW(dgm::Path<dgm::WorldNavpoint>(LVD_PATH));
		}
	}

	TEST_CASE("Traversal", "Path") {
		SECTION("Non-looping") {
			dgm::Path path(POINTS, false);
			
			REQUIRE(not path.isLooping());
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(path.isTraversed());
		}

		SECTION("Looping") {
			dgm::Path path(POINTS, true);
			REQUIRE(path.isLooping());
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(not path.isTraversed());
			REQUIRE(path.getCurrentPoint().coord.x == 2ul);
			REQUIRE(path.getCurrentPoint().coord.y == 0ul);
			path.advance();
			REQUIRE(not path.isTraversed());
		}

		SECTION("Non-looping, constructed from LevelD") {
			dgm::Path<dgm::WorldNavpoint> path(LVD_PATH);

			path.advance();
			REQUIRE(not path.isTraversed());
			path.advance();
			REQUIRE(not path.isTraversed());
			REQUIRE(path.getCurrentPoint().coord.x == 1000.f);
			REQUIRE(path.getCurrentPoint().coord.y == 2000.f);
			path.advance();
			REQUIRE(path.isTraversed());
		}
	}

}
