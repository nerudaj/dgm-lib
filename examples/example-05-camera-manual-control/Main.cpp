#include "DemoData.hpp"
#include <DGM/dgm.hpp>

const float SPEED = 128.f;

// This function is copied from example-01-tilemap
dgm::TileMap createSampleTilemap(const dgm::ResourceManager& resmgr)
{
    auto&& tilemap = dgm::TileMap(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));

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
    auto&& camera = dgm::Camera(
        { { 0.f, 0.f }, { 1.f, 1.f } }, sf::Vector2f(window.getSize()));

    window.getSfmlWindowContext().setFramerateLimit(60);

    float zoomLevel = 1.f;
    float rotation = 0.f;
    dgm::Time time;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                std::ignore = window.close();
            else if (const auto key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Up) zoomLevel -= 0.1f;
                if (key->code == sf::Keyboard::Key::Down) zoomLevel += 0.1f;
                if (key->code == sf::Keyboard::Key::Space)
                    camera.shake(sf::seconds(2.f), 30.f);
                if (key->code == sf::Keyboard::Key::Q) rotation -= 0.5f;
                if (key->code == sf::Keyboard::Key::E) rotation += 0.5f;
            }
        }

        const auto forward = sf::Vector2f {
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)   ? -SPEED
            : sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ? SPEED
                                                               : 0.f,
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)   ? -SPEED
            : sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ? SPEED
                                                               : 0.f,
        };

        playerDot.move(forward * time.getDeltaTime());

        // First center camera on the player dot
        camera.setPosition(playerDot.getPosition());

        // Call update so the shake effect can evaluate
        camera.update(time);
        // WARNING: Player is unable to move while the camera is shaking

        // Apply rest of the transforms
        camera.setRotation(sf::degrees(rotation));
        camera.setZoom(zoomLevel);

        // In each frame, update sf::RenderWindow's view with current camera
        // view
        window.setViewFromCamera(camera);

        // And just render as normal
        window.clear();

        window.draw(tilemap);
        playerDot.debugRender(window);

        window.display();
        time.reset();
    }
}