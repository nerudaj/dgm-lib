#include "DemoData.hpp"
#include "EffectBloodSpatter.hpp"
#include "EffectStarfield.hpp"
#include "EffectTexturedSmoke.hpp"
#include "EffectWaterFountain.hpp"
#include "ParticleEffectBase.hpp"
#include <DGM/dgm.hpp>
#include <ranges>

const sf::Vector2u WINDOW_SIZE_U = { 1600, 900 };
const sf::Vector2f WINDOW_SIZE_F = sf::Vector2f(WINDOW_SIZE_U);

consteval unsigned long long operator""_numparticles(unsigned long long value)
{
    return value;
}

sf::RectangleShape createVisualContainer(unsigned x, unsigned y)
{
    const sf::Vector2f BOX_SIZE = { 350.f, 405.f };
    const sf::Vector2f BOX_OFFSET = {
        (WINDOW_SIZE_F.x - 4.f * BOX_SIZE.x) / 5.f,
        (WINDOW_SIZE_F.y - 2.f * BOX_SIZE.y) / 3.f
    };

    auto&& shape = sf::RectangleShape(BOX_SIZE);
    shape.setOutlineThickness(3.f);
    shape.setOutlineColor(sf::Color::White);
    shape.setFillColor(sf::Color::Transparent);
    shape.setPosition({ BOX_OFFSET.x + x * (BOX_OFFSET.x + BOX_SIZE.x),
                        BOX_OFFSET.y + y * (BOX_OFFSET.y + BOX_SIZE.y) });
    return shape;
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    auto&& window =
        dgm::Window(WINDOW_SIZE_U, "Example: Particle Effects", false);
    dgm::Time time;

    // Images & configs
    auto&& resmgr = DemoData::loadDemoResources();

    // Spawn 8 "containers" for effects
    // It could be all written more nicely, but the goal here was to have
    // effect with interfaces that are compatible with real world project so
    // they can be copy-pasted without many modifications
    auto&& boxes = std::vector<sf::RectangleShape>(4u);
    for (auto&& [idx, box] : std::views::enumerate(boxes))
    {
        box = createVisualContainer(idx, 0u);
    }

    // Create actual effects
    EffectWaterFountain effectFountain(
        256_numparticles,
        { boxes[0].getGlobalBounds().position.x
              + boxes[0].getGlobalBounds().size.x / 2.f,
          boxes[0].getGlobalBounds().position.y
              + boxes[0].getGlobalBounds().size.y });

    EffectBloodSpatter effectBloodSpatter(
        128_numparticles,
        boxes[1].getGlobalBounds().getCenter(),
        boxes[1].getGlobalBounds().position.y
            + boxes[1].getGlobalBounds().size.y);

    EffectStarfield effectStarfield(256, boxes[2].getGlobalBounds());

    EffectTexturedSmoke effectTexturedSmoke(
        256_numparticles,
        { boxes[3].getGlobalBounds().position.x
              + boxes[3].getGlobalBounds().size.x / 2.f,
          boxes[3].getGlobalBounds().position.y
              + boxes[3].getGlobalBounds().size.y - 64.f },
        dgm::Clip({ 256, 256 }, { { 0, 0 }, { 1280, 768 } }));
    effectTexturedSmoke.setTexture(resmgr.get<sf::Texture>("smoke.png"));

    // Create decorations
    auto&& soldierSprite = sf::Sprite(resmgr.get<sf::Texture>("soldier.png"));
    soldierSprite.setOrigin({ 160.f, 160.f });
    soldierSprite.setPosition({ boxes[1].getGlobalBounds().position.x
                                    + boxes[1].getGlobalBounds().size.x / 2.f,
                                boxes[1].getGlobalBounds().position.y
                                    + boxes[1].getGlobalBounds().size.y
                                    - 160.f });

    const auto& soldierAnimationStates =
        resmgr.get<dgm::AnimationStates>("soldier_config.json");
    dgm::Animation soldierAnimation(soldierAnimationStates);
    soldierAnimation.setState("idle", true);
    soldierAnimation.setSpeed(4);

    auto&& starshipSprite = sf::Sprite(resmgr.get<sf::Texture>("starship.png"));
    starshipSprite.setOrigin(sf::Vector2f(64.f, 53.f) / 2.f);
    starshipSprite.setPosition(boxes[2].getGlobalBounds().getCenter());
    starshipSprite.setRotation(sf::degrees(-45.f));

    // FPS counter
    const sf::Time FPS_DISPLAY_UPDATE_FREQUENCY = sf::seconds(0.1f);
    float fpsElapsedSum = 0.f;
    unsigned fpsCount = 0;
    sf::Time fpsTimer = sf::Time::Zero;
    auto&& fpsOutput = sf::Text(resmgr.get<sf::Font>("cruft.ttf"));
    fpsOutput.setFillColor(sf::Color::Yellow);

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
        window.clear();

        effectFountain.draw(window);

        soldierSprite.setTextureRect(
            soldierAnimationStates.at(soldierAnimation.getStateName())
                .getFrame(soldierAnimation.getCurrentFrameIndex()));
        window.draw(soldierSprite);
        effectBloodSpatter.draw(window);

        window.draw(starshipSprite);
        effectStarfield.draw(window);

        effectTexturedSmoke.draw(window);

        for (auto&& box : boxes)
            window.draw(box);

        window.draw(fpsOutput);

        window.display();
    }

    return 0;
}
