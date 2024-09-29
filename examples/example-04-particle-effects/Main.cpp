#include "EffectBloodSpatter.hpp"
#include "EffectStarfield.hpp"
#include "EffectTexturedSmoke.hpp"
#include "EffectWaterFountain.hpp"
#include "ParticleEffectBase.hpp"
#include <DGM/dgm.hpp>

const sf::Vector2u WINDOW_SIZE_U = { 1600, 900 };
const sf::Vector2f WINDOW_SIZE_F = sf::Vector2f(WINDOW_SIZE_U);
const sf::Vector2f BOX_SIZE = { 350.f, 405.f };
const sf::Vector2f BOX_OFFSET = { (WINDOW_SIZE_F.x - 4.f * BOX_SIZE.x) / 5.f,
                                  (WINDOW_SIZE_F.y - 2.f * BOX_SIZE.y) / 3.f };

int main()
{
    srand(time(nullptr));

    auto&& window =
        dgm::Window(WINDOW_SIZE_U, "Example: Particle Effects", false);
    dgm::Time time;

    // Images & configs
    dgm::ResourceManager resmgr;
    std::ignore = resmgr.loadResourcesFromDirectory<sf::Font>(
        RESOURCE_DIR,
        [](const std::filesystem::path& path, sf::Font& font)
        {
            if (!font.loadFromFile(path.string()))
                throw std::runtime_error(path.string());
        },
        { ".ttf" });
    std::ignore = resmgr.loadResourcesFromDirectory<sf::Texture>(
        RESOURCE_DIR,
        [](const std::filesystem::path& path, sf::Texture& texture)
        {
            if (!texture.loadFromFile(path.string()))
                throw std::runtime_error(path.string());
        },
        { ".png" });
    std::ignore = resmgr.loadResourcesFromDirectory<dgm::AnimationStates>(
        RESOURCE_DIR,
        [](const std::filesystem::path& path, dgm::AnimationStates& animations)
        {
            dgm::JsonLoader loader;
            animations = loader.loadAnimationsFromFile(path);
        },
        { ".json" });

    // Spawn 8 "containers" for effects
    // It could be all written more nicely, but the goal here was to have effect
    // with interfaces that are compatible with real world project so they can
    // be copy-pasted without many modifications
    std::vector<sf::RectangleShape> boxes;
    for (unsigned y = 0; y < 2; y++)
    {
        for (unsigned x = 0; x < 4; x++)
        {
            boxes.push_back(sf::RectangleShape(BOX_SIZE));
            boxes.back().setOutlineThickness(3.f);
            boxes.back().setOutlineColor(sf::Color::White);
            boxes.back().setFillColor(sf::Color::Transparent);
            boxes.back().setPosition(
                BOX_OFFSET.x + x * (BOX_OFFSET.x + BOX_SIZE.x),
                BOX_OFFSET.y + y * (BOX_OFFSET.y + BOX_SIZE.y));
        }
    }

    // Create actual effects
    EffectWaterFountain effectFountain(
        256,
        { boxes[0].getGlobalBounds().left
              + boxes[0].getGlobalBounds().width / 2.f,
          boxes[0].getGlobalBounds().top + boxes[0].getGlobalBounds().height });

    const sf::Vector2f BOX1_CENTER = {
        boxes[1].getGlobalBounds().left
            + boxes[1].getGlobalBounds().width / 2.f,
        boxes[1].getGlobalBounds().top + boxes[1].getGlobalBounds().height / 2.f
    };
    EffectBloodSpatter effectBloodSpatter(
        128,
        BOX1_CENTER,
        boxes[1].getGlobalBounds().top + boxes[1].getGlobalBounds().height);

    EffectStarfield effectStarfield(256, boxes[2].getGlobalBounds());

    EffectTexturedSmoke effectTexturedSmoke(
        256,
        { boxes[3].getGlobalBounds().left
              + boxes[3].getGlobalBounds().width / 2.f,
          boxes[3].getGlobalBounds().top + boxes[3].getGlobalBounds().height
              - 64.f },
        dgm::Clip({ 256, 256 }, { 0, 0, 1280, 768 }));
    effectTexturedSmoke.setTexture(
        resmgr.get<sf::Texture>("smoke.png").value().get());

    // Create decorations
    sf::Sprite soldierSprite;
    soldierSprite.setTexture(
        resmgr.get<sf::Texture>("soldier.png").value().get());
    soldierSprite.setOrigin(160.f, 160.f);
    soldierSprite.setPosition(
        boxes[1].getGlobalBounds().left
            + boxes[1].getGlobalBounds().width / 2.f,
        boxes[1].getGlobalBounds().top + boxes[1].getGlobalBounds().height
            - 160.f);

    dgm::Animation soldierAnimation(
        resmgr.get<dgm::AnimationStates>("soldier_config.json").value().get());
    soldierAnimation.setState("idle", true);
    soldierAnimation.setSpeed(4);

    sf::Sprite starshipSprite;
    starshipSprite.setTexture(
        resmgr.get<sf::Texture>("starship.png").value().get());
    starshipSprite.setOrigin(sf::Vector2f(64.f, 53.f) / 2.f);
    starshipSprite.setPosition(
        boxes[2].getGlobalBounds().left
            + boxes[2].getGlobalBounds().width / 2.f,
        boxes[2].getGlobalBounds().top
            + boxes[2].getGlobalBounds().height / 2.f);
    starshipSprite.setRotation(-45.f);

    // FPS counter
    const sf::Time FPS_DISPLAY_UPDATE_FREQUENCY = sf::seconds(0.1f);
    float fpsElapsedSum = 0.f;
    unsigned fpsCount = 0;
    sf::Time fpsTimer = sf::Time::Zero;
    sf::Text fpsOutput;
    fpsOutput.setFont(resmgr.get<sf::Font>("cruft.ttf").value().get());
    fpsOutput.setFillColor(sf::Color::Yellow);

    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                std::ignore = window.close();
            }
        }

        /* LOGIC */
        time.reset();

        fpsCount++;
        fpsElapsedSum += time.getDeltaTime();
        fpsTimer += time.getElapsed();
        if (fpsTimer >= FPS_DISPLAY_UPDATE_FREQUENCY)
        {
            const float averageFps = fpsElapsedSum / fpsCount;
            fpsOutput.setString(
                std::to_string(static_cast<unsigned>(1.f / averageFps)));
            fpsCount = 0;
            fpsElapsedSum = 0;
            fpsTimer = sf::Time::Zero;
        }

        soldierAnimation.update(time);

        effectFountain.update(time);
        effectBloodSpatter.update(time);
        if (effectBloodSpatter.finished()) effectBloodSpatter.reset();

        effectStarfield.update(time);

        effectTexturedSmoke.update(time);

        /* DRAW */
        window.beginDraw();

        effectFountain.draw(window);

        soldierSprite.setTextureRect(soldierAnimation.getCurrentFrame());
        window.draw(soldierSprite);
        effectBloodSpatter.draw(window);

        window.draw(starshipSprite);
        effectStarfield.draw(window);

        effectTexturedSmoke.draw(window);

        for (auto&& box : boxes)
            window.draw(box);

        window.draw(fpsOutput);

        window.endDraw();
    }

    return 0;
}