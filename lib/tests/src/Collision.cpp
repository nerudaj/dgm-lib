#include <catch.hpp>
#include <DGM/dgm.hpp>

TEST_CASE("Rect-to-point", "Collision") {
	dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

	SECTION("No collision") {
		sf::Vector2i point(-1, -1);
		REQUIRE_FALSE(dgm::Collision::basic(rect, point));
	}

	SECTION("Normal collision") {
		sf::Vector2i point(5, 5);
		REQUIRE(dgm::Collision::basic(rect, point));
	}
}

TEST_CASE("Rect-to-rect", "Collision") {
	dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

	SECTION("No collision") {
		dgm::Rect rect2(-20.f, -20.f, 10.f, 10.f);
		REQUIRE_FALSE(dgm::Collision::basic(rect, rect2));
	}
	
	SECTION("Normal collision") {
		dgm::Rect rect2(-5.f, -5.f, 10.f, 10.f);
		REQUIRE(dgm::Collision::basic(rect, rect2));
	}
}

TEST_CASE("Rect-to-circle", "Collision") {
	dgm::Rect rect(0.f, 0.f, 10.f, 10.f);

	SECTION("No collision") {
		dgm::Circle circle(-20.f, -20.f, 10.f);
		REQUIRE_FALSE(dgm::Collision::basic(rect, circle));
	}
	
	SECTION("Normal collision") {
		dgm::Circle circle(-5.f, -5.f, 10.f);
		REQUIRE(dgm::Collision::basic(rect, circle));
	}
}

TEST_CASE("Circle-to-circle", "Collision") {
	dgm::Circle circle1(5.f, 5.f, 5.f);

	SECTION("No collision") {
		dgm::Circle circle2(-5.f, -5.f, 5.f);
		REQUIRE_FALSE(dgm::Collision::basic(circle1, circle2));
	}

	SECTION("Normal collision") {
		dgm::Circle circle2(0.f, 0.f, 5.f);
		REQUIRE(dgm::Collision::basic(circle1, circle2));
	}
}

TEST_CASE("Circle-to-point", "Collision") {
	dgm::Circle circle(5.f, 5.f, 5.f);

	SECTION("No collision") {
		sf::Vector2i point(0, 0);
		REQUIRE_FALSE(dgm::Collision::basic(circle, point));
	}

	SECTION("Normal collision") {
		sf::Vector2i point(4, 4);
		REQUIRE(dgm::Collision::basic(circle, point));
	}
}

TEST_CASE("Mesh-to-rect", "Collision") {
	dgm::Mesh mesh;
	mesh.setVoxelSize(10.f, 10.f);
	mesh.setDataSize(2, 2);
	mesh.at(0, 0) = 1;
	mesh.at(1, 0) = 0;
	mesh.at(0, 1) = 1;
	mesh.at(1, 1) = 1;

	SECTION("No collision") {
		dgm::Rect rect(11.f, 1.f, 8.f, 8.f);
		std::size_t hitPos = 0;
		REQUIRE_FALSE(dgm::Collision::basic(mesh, rect, &hitPos));
		REQUIRE(hitPos == 0);
	}

	SECTION("Normal collision") {
		dgm::Rect rect(11.f, 11.f, 8.f, 8.f);
		std::size_t hitPos = 0;
		REQUIRE(dgm::Collision::basic(mesh, rect, &hitPos));
		REQUIRE(hitPos == 3);
	}
}

TEST_CASE("Mesh-to-circle", "Collision") {
	dgm::Mesh mesh;
	mesh.setVoxelSize(10.f, 10.f);
	mesh.setDataSize(2, 2);
	mesh.at(0, 0) = 1;
	mesh.at(1, 0) = 0;
	mesh.at(0, 1) = 1;
	mesh.at(1, 1) = 1;

	SECTION("No collision") {
		dgm::Circle circle(15.f, 5.f, 4.f);
		std::size_t hitPos = 0;
		REQUIRE_FALSE(dgm::Collision::basic(mesh, circle, &hitPos));
		REQUIRE(hitPos == 0);
	}

	SECTION("Normal collision") {
		dgm::Circle circle(15.f, 10.f, 4.f);
		std::size_t hitPos = 0;
		REQUIRE(dgm::Collision::basic(mesh, circle, &hitPos));
		REQUIRE(hitPos == 3);
	}
}

// NOTE: This test doesn't work because catch wrongly treats
// pass-by-reference parameters
/*
TEST_CASE("Advanced Mesh-to-rect", "Collision") {
	dgm::Mesh mesh;
	mesh.setVoxelSize(10.f, 10.f);
	mesh.setDataSize(2, 2);
	mesh.at(0, 0) = 1;
	mesh.at(1, 0) = 0;
	mesh.at(0, 1) = 1;
	mesh.at(1, 1) = 1;


	dgm::Rect rect(11.f, 11.f, 8.f, 8.f);
	sf::Vector2f fwd(0.f, 10.f);
	REQUIRE_FALSE(dgm::Collision::advanced(mesh, rect, fwd));
	REQUIRE(fwd.y == 0.f);
	REQUIRE(rect.getPosition().y == 2.f);
}
*/
