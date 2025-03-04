/*
Move the yellow dot by pressing WASD.
Top-left corner shows a minimap of the level.
*/

#include "DemoData.hpp"
#include "SimpleController.hpp"
#include <DGM/dgm.hpp>

dgm::DynamicBuffer<dgm::Rect> spawnCherries()
{
    const std::vector<sf::Vector2f> positions = {
        { 96.f, 512.f },  { 363.f, 290.f }, { 668.f, 384.f }, { 839.f, 734.f },
        { 542.f, 622.f }, { 393.f, 778.f }, { 159.f, 757.f }, { 287.f, 310.f },
    };
    const sf::Vector2f size = { 16.f, 16.f };

    auto&& cherries = dgm::DynamicBuffer<dgm::Rect>(16);

    for (auto&& position : positions)
        cherries.emplaceBack(dgm::Rect(position, size));

    return cherries;
}

void handleCollisionsWithCherries(
    const dgm::Circle& player, dgm::DynamicBuffer<dgm::Rect>& cherries)
{
    std::optional<size_t> cherryIdxToDelete;

    for (auto&& [cherry, idx] : cherries)
    {
        if (dgm::Collision::basic(cherry, player))
        {
            cherryIdxToDelete = idx;
            break;
        }
    }

    if (cherryIdxToDelete) cherries.eraseAtIndex(*cherryIdxToDelete);
}

sf::FloatRect computeMinimapRect(
    const sf::Vector2u& minimapResolution, const sf::Vector2u& windowResolution)
{
    const auto size = sf::Vector2f(
        static_cast<float>(minimapResolution.x) / windowResolution.x,
        static_cast<float>(minimapResolution.y) / windowResolution.y);

    return sf::FloatRect({ 0.f, 0.f }, size);
}

sf::Vector2u computeMeshBoundingBoxSize(const dgm::Mesh& mesh)
{
    return mesh.getDataSize().componentWiseMul(mesh.getVoxelSize());
}

int main()
{
    auto&& window =
        dgm::Window({ 1280, 720 }, "Example: Camera minimap", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& level = DemoData::createDemoLevel(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));
    auto&& input = SimpleController();
    dgm::Time time;

    auto&& player = dgm::Circle({ 96.f, 96.f }, 16.f);
    sf::Vector2f forward;
    const float SPEED = 256.f; // px per second

    // Main camera is for rendering the scene, level, player, etc
    auto&& mainCamera = dgm::Camera(
        sf::FloatRect({ 0.f, 0.f }, { 1.f, 1.f }),
        sf::Vector2f(window.getSize()));

    // This camera is only for rendering minimap
    // This camera resolution is the same as size of the level mesh, so it can
    // be rendered fully
    auto&& minimapCamera = dgm::Camera(
        computeMinimapRect(sf::Vector2u(196, 196), window.getSize()),
        sf::Vector2f(computeMeshBoundingBoxSize(level.getMesh())));

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
        mainCamera.setPosition(player.getPosition());

        /* DRAW */
        window.clear();

        { // First, render the main scene
            window.setViewFromCamera(mainCamera);
            level.draw(window);
            player.debugRender(window);
        }

        { // And then, render the minimap over it
            window.setViewFromCamera(minimapCamera);
            level.draw(window);
        }

        window.display();
    }

    return 0;
}
