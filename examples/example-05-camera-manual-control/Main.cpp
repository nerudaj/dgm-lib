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

enum Action
{
    Up,
    Down,
    Left,
    Right,
    Shake,
    RotateLeft,
    RotateRight,
    ZoomIn,
    ZoomOut,
};

dgm::Controller setupController()
{
    auto&& input = dgm::Controller();
    input.bindInput(Up, sf::Keyboard::W);
    input.bindInput(Down, sf::Keyboard::S);
    input.bindInput(Left, sf::Keyboard::A);
    input.bindInput(Right, sf::Keyboard::D);
    input.bindInput(Shake, sf::Keyboard::Space);
    input.bindInput(RotateLeft, sf::Keyboard::Q);
    input.bindInput(RotateRight, sf::Keyboard::E);
    input.bindInput(ZoomIn, sf::Keyboard::Up);
    input.bindInput(ZoomOut, sf::Keyboard::Down);

    return input;
}

void updatePlayer(dgm::Circle& playerDot, dgm::Controller& input)
{
    auto forward = sf::Vector2f {
        input.getInputValue(Left) * -1.f + input.getInputValue(Right),
        input.getInputValue(Up) * -1.f + input.getInputValue(Down),
    };

    playerDot.move(forward * 3.f); // FPS fixed to 60, so 180px per second
}

int main(int, char*[])
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Camera", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& tilemap = createSampleTilemap(resmgr);
    auto&& playerDot = dgm::Circle({ 100.f, 100.f }, 8.f);
    auto&& input = setupController();

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
            if (event.type == sf::Event::Closed) std::ignore = window.close();
        }

        updatePlayer(playerDot, input);

        // Handle camera-related effects
        if (input.isInputToggled(RotateLeft))
            rotation -= 0.5f;
        else if (input.isInputToggled(RotateRight))
            rotation += 0.5f;

        if (input.isInputToggled(ZoomIn))
            zoomLevel -= 0.01f;
        else if (input.isInputToggled(ZoomOut))
            zoomLevel += 0.01f;

        if (input.isInputToggled(Shake))
        {
            input.releaseInput(Shake);
            camera.shake(sf::seconds(2.f), 30.f);
        }

        // First center camera on the player dot
        camera.setPosition(playerDot.getPosition());

        // Call update so the shake effect can evaluate
        camera.update(time);

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