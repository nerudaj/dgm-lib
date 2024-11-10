#include "DemoData.hpp"
#include <DGM/dgm.hpp>

// This function is copied from example-01-tilemap
dgm::TileMap createSampleTilemap(const dgm::ResourceManager& resmgr)
{
    auto&& tilemap = dgm::TileMap(
        resmgr.get<sf::Texture>("tileset.png").value().get(),
        resmgr.get<dgm::Clip>("tileset.png.clip").value().get());

    // clang-format off
    tilemap.build(
        {64u, 64u},
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 2, 2, 1, 
            1, 0, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        },
        {15, 10});
    // clang-format on

    return tilemap;
}

int main(int, char*[])
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Camera", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& tilemap = createSampleTilemap(resmgr);
    auto&& playerDot = dgm::Circle({ 100.f, 100.f }, 8.f);

    // Make the camera take up the whole screen and use native window resolution
    auto&& camera =
        dgm::Camera({ 0.f, 0.f, 1.f, 1.f }, sf::Vector2f(window.getSize()));

    window.getWindowContext().setFramerateLimit(60);

    float zoomLevel = 1.f;
    float rotation = 0.f;
    dgm::Time time;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                std::ignore = window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W) playerDot.move(0, -3.f);
                if (event.key.code == sf::Keyboard::A)
                    playerDot.move(-3.f, 0.f);
                if (event.key.code == sf::Keyboard::S) playerDot.move(0, 3.f);
                if (event.key.code == sf::Keyboard::D) playerDot.move(3.f, 0.f);
                if (event.key.code == sf::Keyboard::Up) zoomLevel -= 0.1f;
                if (event.key.code == sf::Keyboard::Down) zoomLevel += 0.1f;
                if (event.key.code == sf::Keyboard::Space)
                    camera.shake(sf::seconds(2.f), 30.f);
                if (event.key.code == sf::Keyboard::Q) rotation -= 0.5f;
                if (event.key.code == sf::Keyboard::E) rotation += 0.5f;
            }
        }

        // First center camera on the player dot
        camera.setPosition(playerDot.getPosition());

        // Call update so the shake effect can evaluate
        camera.update(time);
        // WARNING: Player is unable to move while the camera is shaking

        // Apply rest of the transforms
        camera.setRotation(rotation);
        camera.setZoom(zoomLevel);

        // In each frame, update sf::RenderWindow's view with current camera
        // view
        window.setViewFromCamera(camera);

        // And just render as normal
        window.beginDraw();

        window.draw(tilemap);
        playerDot.debugRender(window);

        window.endDraw();
        time.reset();
    }
}