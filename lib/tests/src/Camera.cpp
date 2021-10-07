#include <catch.hpp>
#include <DGM/dgm.hpp>

class TestableTime_Camera : public dgm::Time {
public:
	TestableTime_Camera(float dt) {
		deltaTime = dt;
	}
};

TEST_CASE("IsObjectVisible", "Camera") {
	sf::View view({10.f, 10.f}, {20.f, 20.f});
	dgm::Camera camera(view);

	SECTION("Fully in view") {
		dgm::Circle circle({ 2.f, 2.f }, 1.f);

		REQUIRE(camera.isObjectVisible(circle));
	}

	SECTION("Partially in view") {
		dgm::Rect rect({ -2.f, -2.f }, {6.f, 6.f});

		REQUIRE(camera.isObjectVisible(rect));
	}

	SECTION("Outside of view") {
		dgm::Rect rect({ -2.f, -2.f }, { 1.9f, 1.9f });

		REQUIRE(!camera.isObjectVisible(rect));
	}
}

TEST_CASE("Move", "Camera") {
	sf::View view({ 10.f, 10.f }, { 20.f, 20.f });
	dgm::Camera camera(view);

	SECTION("Immediate move") {
		camera.move({ 10.f, 15.f });
		REQUIRE(Approx(view.getCenter().x) == 20.f);
		REQUIRE(Approx(view.getCenter().y) == 25.f);
	}

	SECTION("Gradual move") {
		camera.moveGradually({ 20.f, 25.f }, sf::seconds(1.f), [] (float x) -> float { return x; });

		TestableTime_Camera time(0.1f);
		for (unsigned i = 0; i < 10; i++) {
			REQUIRE(camera.isMoving());
			camera.update(time);
		}

		REQUIRE(Approx(view.getCenter().x) == 20.f);
		REQUIRE(Approx(view.getCenter().y) == 25.f);
	}
}