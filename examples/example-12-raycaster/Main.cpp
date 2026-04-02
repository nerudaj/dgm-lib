/*
Move the yellow dot by pressing WASD.
Raycasts are spawned from the yellow dot to the red dots, showed as white lines.
If raycast has direct visibility to the target (doesn't hit any walls), line is
green instead of white.
*/

#include "DemoData.hpp"
#include "Helpers.hpp"
#include "SimpleController.hpp"
#include <DGM/classes/Raycaster.hpp>
#include <DGM/dgm.hpp>

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Collisions", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& level = DemoData::createDemoLevel(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));
    auto&& input = SimpleController();
    dgm::Time time;

    auto&& player = dgm::Circle({ 96.f, 96.f }, 16.f);
    sf::Vector2f forward;
    const float SPEED = 256.f; // px per second

    auto&& cherries = DemoData::spawnCherries();

    auto&& camera = dgm::Camera(
        sf::FloatRect({ 0.f, 0.f }, { 1.f, 1.f }),
        sf::Vector2f(window.getSize()));

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
        }

        /* LOGIC */
        time.reset();

        forward = input.getForward() * time.getDeltaTime() * SPEED;

        // advanced collision will try if it possible to move 'player' by
        // 'forward' and if not, it adjusts 'forward' so it can be safely used
        // to move 'player' and not end up stuck in the level mesh
        dgm::Collision::advanced(level.getMesh(), player, forward);
        player.move(forward);
        camera.setPosition(player.getPosition());

        /* DRAW */
        window.setViewFromCamera(camera);
        window.clear();

        level.draw(window);
        player.debugRender(window);

        for (auto&& [cherry, _] : cherries)
        {
            cherry.debugRender(window, sf::Color::Red);

            const auto raycast = dgm::Raycaster::raycast(
                player.getPosition(),
                cherry.getCenter() - player.getPosition(),
                level.getMesh());
            drawLine(
                window,
                player.getPosition(),
                raycast.hitLocation,
                sf::Color::White);

            if (dgm::Raycaster::hasDirectVisibility(
                    player.getPosition(), cherry.getCenter(), level.getMesh()))
            {
                drawLine(
                    window,
                    cherry.getCenter(),
                    player.getPosition(),
                    sf::Color::Green);
            }
        }

        window.display();
    }

    return 0;
}
