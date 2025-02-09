#pragma once

#include "ParticleEffectBase.hpp"

class EffectStarfield : public ParticleEffectBase
{
protected:
    // Effect parameters
    const sf::Time SPAWN_TIMEOUT = sf::seconds(0.01f);
    const float PARTICLE_BASE_SPEED = 32.f;

    // Initialized from ctor
    const dgm::Rect bounds;

    // Vars
    sf::Time spawnTimeout = SPAWN_TIMEOUT;

protected:
    void spawnParticle()
    {
        if (!particles.grow()) return;

        const float speedSizeFactor = getRandomFloat(1.f, 5.f);

        // This example is more complicated because it needs to spawn particles
        // only within given bounding box while normally you would simply render
        // the particles outside of the screen so you can change direction
        // easily In this case we opt for fixed direction and generate particles
        // along the top and right edges of the bounds
        const float randomX = getRandomFloat(
            bounds.getPosition().x,
            bounds.getPosition().x + bounds.getSize().x);
        const float randomY = getRandomFloat(
            bounds.getPosition().y,
            bounds.getPosition().y + bounds.getSize().y);
        const float startX = bounds.getPosition().x + bounds.getSize().x - 1.f;
        const float startY = bounds.getPosition().y + 1.f;
        const bool spawnOnTop = static_cast<bool>(rand() % 2);
        const sf::Vector2f randomPosition = spawnOnTop
                                                ? sf::Vector2f(randomX, startY)
                                                : sf::Vector2f(startX, randomY);

        auto& p = particles.getLast();
        p->setColor(sf::Color::White);
        p->spawn(
            randomPosition,
            sf::Vector2f(1.f, 1.f) * speedSizeFactor,
            sf::Time::Zero);
        p->setForward(sf::Vector2f(
            -PARTICLE_BASE_SPEED * speedSizeFactor, sf::degrees(-45.f)));
    }

public:
    virtual void update(const dgm::Time& time) override
    {
        spawnTimeout -= time.getElapsed();
        if (spawnTimeout <= sf::Time::Zero)
        {
            spawnTimeout = SPAWN_TIMEOUT;
            spawnParticle();
        }

        for (unsigned i = 0; i < particles.getSize(); i++)
        {
            particles[i]->moveForwardBy(
                particles[i]->getForward() * time.getDeltaTime());

            if (!dgm::Collision::basic(
                    bounds, sf::Vector2i(particles[i]->getPosition())))
            {
                particles[i]->despawn();
                particles.remove(i--);
            }
        }
    }

    EffectStarfield(unsigned particleCount, const sf::FloatRect& bounds)
        : ParticleEffectBase(particleCount)
        , bounds(bounds.position, bounds.size)
    {
    }
};
