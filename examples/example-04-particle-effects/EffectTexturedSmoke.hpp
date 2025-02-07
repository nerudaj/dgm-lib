#pragma once

#include "ParticleEffectBase.hpp"

class EffectTexturedSmoke : public ParticleEffectBase
{
protected:
    // Parameters
    const sf::Time SPAWN_TIMEOUT = sf::seconds(0.1f);
    const sf::Vector2f PARTICLE_SIZE = sf::Vector2f(128.f, 128.f);
    const float MIN_LIFESPAN = 1.f; // in seconds
    const float MAX_LIFESPAN = 2.f; // in seconds
    const float MIN_EMITTER_FORCE = 60.f;
    const float MAX_EMITTER_FORCE = 80.f;
    const float MIN_EMITTED_SPREAD = -90.f; // degrees
    const float MAX_EMITTER_SPREAD = 90.f;  // degrees

    // Set by ctor
    const sf::Vector2f emitterPosition;
    const dgm::Clip clip;

    // Vars
    sf::Time spawnTimer = SPAWN_TIMEOUT;

protected:
    void spawnParticle()
    {
        if (not particles.grow()) return;

        static int frameIndex = 0;

        auto& p = particles.getLast();

        const sf::Vector2f BASE_DIR(0.f, -1.f);
        const auto randomDirection = BASE_DIR.rotatedBy(sf::degrees(
            getRandomFloat(MIN_EMITTED_SPREAD, MAX_EMITTER_SPREAD)));
        const auto randomSpeed =
            getRandomFloat(MIN_EMITTER_FORCE, MAX_EMITTER_FORCE);
        const float randomLifespan = getRandomFloat(MIN_LIFESPAN, MAX_LIFESPAN);

        p->spawn(emitterPosition, PARTICLE_SIZE, sf::seconds(randomLifespan));
        p->setAnimationFrame(clip.getFrame(frameIndex));
        p->setForward(randomDirection * randomSpeed);

        ++frameIndex;
        if (frameIndex + 1 == clip.getFrameCount()) frameIndex = 0;
    }

public:
    virtual void update(const dgm::Time& time)
    {
        for (unsigned i = 0; i < particles.getSize(); i++)
        {
            auto& p = particles[i];
            p->moveForwardBy(p->getForward() * time.getDeltaTime());
            p->updateLifespan(time.getElapsed());

            if (!p->isAlive())
            {
                p->despawn();
                particles.remove(i--);
            }
        }

        spawnTimer -= time.getElapsed();
        if (spawnTimer <= sf::Time::Zero)
        {
            spawnTimer = SPAWN_TIMEOUT;
            spawnParticle();
        }
    }

    EffectTexturedSmoke(
        unsigned particleCount,
        const sf::Vector2f& emitterPosition,
        const dgm::Clip& clip)
        : ParticleEffectBase(particleCount)
        , emitterPosition(emitterPosition)
        , clip(clip)
    {
    }
};