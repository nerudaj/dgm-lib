// An example that showcases how a texture atlas can be utilized

#include "DemoData.hpp"
#include <DGM/dgm.hpp>

class AnimatedSprite
{
public:
    AnimatedSprite(
        const sf::Texture& texture, const dgm::AnimationStates& states)
        : sprite(texture), animationStates(states), animation(states, 15)
    {
    }

public:
    void update(const dgm::Time& time)
    {
        animation.update(time);
        sprite.setTextureRect(animationStates.at(animation.getStateName())
                                  .getFrame(animation.getCurrentFrameIndex()));
    }

    void draw(dgm::Window& window)
    {
        window.draw(sprite);
    }

private:
    sf::Sprite sprite;
    const dgm::AnimationStates& animationStates;
    dgm::Animation animation;
};

int main()
{
    auto&& window = dgm::Window({ 1280, 720 }, "Example: Texture atlas", false);
    auto&& resmgr = DemoData::loadDemoResources();
    auto&& atlas = dgm::TextureAtlas(1920, 1920);

    auto&& tilesetLoc = atlas.addTileset(
        resmgr.get<sf::Texture>("tileset.png"),
        resmgr.get<dgm::Clip>("tileset.png.clip"));

    auto&& spritesheetLoc = atlas.addSpritesheet(
        resmgr.get<sf::Texture>("soldier.png"),
        resmgr.get<dgm::AnimationStates>("soldier_config.json"));

    std::ignore = atlas.getTexture().copyToImage().saveToFile("atlas.png");

    auto&& level = DemoData::createDemoLevel(
        atlas.getTexture(), atlas.getClip(tilesetLoc.value()));
    auto&& sprite = AnimatedSprite(
        atlas.getTexture(), atlas.getAnimationStates(spritesheetLoc.value()));

    dgm::Time time;

    while (window.isOpen())
    {
        time.reset();

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
            }
        }

        sprite.update(time);

        window.clear();

        level.draw(window);
        sprite.draw(window);

        window.display();
    }

    return 0;
}
