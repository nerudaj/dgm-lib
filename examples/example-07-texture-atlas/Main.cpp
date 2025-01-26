#include "DemoData.hpp"
#include <DGM/dgm.hpp>

class AnimatedSprite
{
public:
    AnimatedSprite(
        const sf::Texture& texture, const dgm::AnimationStates& states)
        : animation(states, 15)
    {
        sprite.setTexture(texture);
    }

public:
    void update(const dgm::Time& time)
    {
        animation.update(time);
        sprite.setTextureRect(animation.getCurrentFrame());
    }

    void draw(dgm::Window& window)
    {
        window.draw(sprite);
    }

private:
    sf::Sprite sprite;
    dgm::Animation animation;
};

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Texture atlas", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& atlas = dgm::TextureAtlas(1920, 1920);

    auto&& tilesetLoc = atlas.addTileset(
        resmgr.get<sf::Texture>("tileset.png").value().get(),
        resmgr.get<dgm::Clip>("tileset.png.clip").value().get());

    auto&& spritesheetLoc = atlas.addSpritesheet(
        resmgr.get<sf::Texture>("soldier.png").value().get(),
        resmgr.get<dgm::AnimationStates>("soldier_config.json").value().get());

    auto&& level = DemoData::createDemoLevel(
        atlas.getTexture(), atlas.getClip(tilesetLoc.value()));
    auto&& sprite = AnimatedSprite(
        atlas.getTexture(), atlas.getAnimationStates(spritesheetLoc.value()));

    dgm::Time time;

    while (window.isOpen())
    {
        time.reset();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                std::ignore = window.close();
            }
        }

        sprite.update(time);

        window.beginDraw();

        level.draw(window);
        sprite.draw(window);

        window.endDraw();
    }

    return 0;
}