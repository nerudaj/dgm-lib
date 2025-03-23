/*
 * example-pathfind - Click anywhere in the map and yellow circle will compute
 * optimal path and move to target spot. Click on invalid spot (outside of the
 * map or a wall) and nothing will happen.
 */

#include "../shared/DemoData.hpp"
#include "../shared/Level.hpp"
#include <DGM/dgm.hpp>

class Actor
{
private:
    const float SPEED = 256.f;
    const float RADIUS = 16.f;

    dgm::Circle body = dgm::Circle({ 100.f, 100.f }, RADIUS);
    dgm::WorldNavMesh navMesh;
    std::optional<dgm::Path<dgm::WorldNavpoint>> path;

    sf::Vector2f oldPosition = { 0.f, 0.f };
    sf::Vector2f forward = { 0.f, 0.f };
    float transitionTimer = 0.f;
    float transitionDuration = 0.f;

    void beginTransitionToNextPoint()
    {
        if (!hasValidPath()) return;

        oldPosition = body.getPosition();
        forward = path->getCurrentPoint().coord - body.getPosition();
        transitionTimer = 0.f;
        transitionDuration = forward.length() / SPEED;
    }

public:
    enum
    {
        Up,
        Left,
        Right,
        Down
    };

    void draw(dgm::Window& window)
    {
        body.debugRender(window);
    }

    void update(const dgm::Time& time)
    {
        if (!hasValidPath()) return;

        transitionTimer += time.getDeltaTime();
        const auto transition =
            std::clamp(transitionTimer / transitionDuration, 0.f, 1.f);
        body.setPosition(oldPosition + forward * transition);

        if (transition >= 1.f)
        {
            path->advance();
            beginTransitionToNextPoint();
        }
    }

    void setWaypoint(const sf::Vector2f& point)
    {
        std::cout << dgm::Utility::to_string(body.getPosition()) << " -> "
                  << dgm::Utility::to_string(point) << std::endl;

        auto result = navMesh.computePath(body.getPosition(), point);
        if (!result.isTraversed()) path = std::move(result);
        beginTransitionToNextPoint();
    }

    bool hasValidPath() const
    {
        return path && !path->isTraversed();
    }

    Actor(const dgm::Mesh& mesh) : navMesh(mesh.clone()) {}
};

int main()
{
    dgm::Window window({ 1280, 980 }, "Example: Pathfind", false);
    dgm::Time time;

    dgm::ResourceManager resmgr = DemoData::loadDemoResources();
    std::ignore = resmgr.loadResourcesFromDirectory<sf::Texture>(
        RESOURCE_DIR,
        [](const std::filesystem::path& path)
            -> std::expected<sf::Texture, dgm::Error>
        { return sf::Texture(path); },
        { ".png" });

    auto level = DemoData::createDemoLevel(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));

    Actor actor(level.getMesh());

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
            else if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto mousePos =
                    sf::Mouse::getPosition(window.getSfmlWindowContext());
                actor.setWaypoint(sf::Vector2f(mousePos));
            }
        }

        /* LOGIC */
        time.reset();

        actor.update(time);

        /* DRAW */
        window.clear();

        level.draw(window);
        actor.draw(window);

        window.display();
    }

    return 0;
}
