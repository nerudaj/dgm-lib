#include <DGM/classes/Animation.hpp>
#include <DemoData.hpp>
#include <catch2/catch_all.hpp>

class MockedTime : public dgm::Time
{
public:
    MockedTime(sf::Time time)
    {
        elapsed = time;
    }
};

TEST_CASE("[Animation]")
{
    auto&& spritesheet = createDummySpritesheet();
    auto&& animation = dgm::Animation(spritesheet);
    auto&& _25msElapsed = MockedTime(sf::Time(std::chrono::milliseconds(25)));

    SECTION("set/getSpeed works")
    {
        animation.setSpeed(42);
        REQUIRE(animation.getSpeed() == 42);
    }

    SECTION("Returns Finished state after clip finished")
    {
        // speed is 30FPS ~ 33.3ms
        animation.setState("first", false); // 2 frame clip

        // Still in frame 1
        REQUIRE(
            animation.update(_25msElapsed)
            == dgm::Animation::PlaybackStatus::Playing);

        // Still in frame 2
        REQUIRE(
            animation.update(_25msElapsed)
            == dgm::Animation::PlaybackStatus::Playing);

        // Ended
        REQUIRE(
            animation.update(_25msElapsed)
            == dgm::Animation::PlaybackStatus::Finished);
    }

    SECTION("Skips multiple states on long enough time jump")
    {
        animation.setSpeed(120);            // 8.3 ms per frame
        animation.setState("first", false); // 2 frame clip
        REQUIRE(
            animation.update(_25msElapsed)
            == dgm::Animation::PlaybackStatus::Finished);
    }

    SECTION("Looping state always loops back")
    {
        animation.setSpeed(120);           // 8.3 ms per frame
        animation.setState("first", true); // 2 frame clip
        REQUIRE(
            animation.update(_25msElapsed)
            == dgm::Animation::PlaybackStatus::Playing);
    }
}