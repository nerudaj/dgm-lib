// Demo having 20'000 particles bouncing from each other
// Optimized through use of dgm::SpatialBuffer and
// custom particle system.

#include <DGM/dgm.hpp>
#include <FpsCounter.hpp>
#include <WhiteRectangleParticle.hpp>

/**
 *  A custom particle system which has many particles bouncing off each other
 * and off the screen boundaries.
 *
 *  It doesn't inhering from ParticleSystemInterface because it needs a
 * SpatialBuffer as a particle storage instead of a StaticBuffer.
 */
class CollidingAgentsParticleSystem
{
public:
    using super = dgm::ps::ParticleSystemInterface;

public:
    CollidingAgentsParticleSystem(unsigned particleCount)
        : renderer(particleCount)
        , particles(dgm::Rect(0.f, 0.f, 1280.f, 1280.f), 640)
    {
        for (unsigned i = 0; i < particleCount; i++)
        {
            auto particle = std::make_unique<WhiteRectangleParticle>(
                renderer.getParticleVertices(i));

            const auto&& x = (i % 160) * 8;
            const auto&& y = (i / 160) * 8;
            const auto&& size = static_cast<float>(i % 5 + 1);
            const auto&& position = sf::Vector2f(x + 4.f, y + 4.f);

            particle->spawn(position, sf::Vector2f(1.f, 1.f) * size);
            particle->setForward(
                dgm::Math::getRotated(
                    sf::Vector2f { 1.f, 0.f }, static_cast<float>(rand() % 360))
                * (6 - size) * 10.f);

            auto bodyCopy = particle->body;
            particles.insert(std::move(particle), bodyCopy);
        }
    }

public:
    void update(const dgm::Time& time)
    {
        for (auto&& [particle, particleId] : particles)
        {
            auto forward = particle->getForward();
            auto&& tickForward = forward * time.getDeltaTime();

            // Object must be removed from lookup before it can be moved
            particles.removeFromLookup(particleId, particle->body);

            // Copying the body so we can move it around and test
            // possible destinations for collisions
            dgm::Circle bodyCpy = particle->body;
            bodyCpy.move(tickForward);
            bool secondCollisionFound = false;

            // Test and handle collision with nearest neighbors
            auto&& candidates = particles.getOverlapCandidates(bodyCpy);
            for (auto&& candidateId : candidates)
            {
                if (dgm::Collision::basic(
                        bodyCpy, particles[candidateId]->body))
                {
                    if (!secondCollisionFound)
                    {
                        tickForward *= -1.f;
                        secondCollisionFound = true;
                        forward *= -1.f;
                    }
                    else
                    {
                        tickForward = { 0.f, 0.f };
                        break;
                    }
                }
            }

            // Handle collision with edge of the screen
            if ((bodyCpy.getPosition().x + tickForward.x) < 0.f
                || (bodyCpy.getPosition().x + tickForward.x) > 1280.f)
            {
                forward.x *= -1.f;
                tickForward.x *= -1.f;
            }

            if ((bodyCpy.getPosition().y + tickForward.y) < 0.f
                || (bodyCpy.getPosition().y + tickForward.y) > 720.f)
            {
                forward.y *= -1.f;
                tickForward.y *= -1.f;
            }

            // Only move particles and their collision boxes after all
            // collisions have been considered
            particle->setForward(forward);
            particle->moveForwardBy(tickForward);

            // Return moved element into the lookup
            particles.returnToLookup(particleId, particle->body);
        }
    }

    void drawTo(dgm::Window& window)
    {
        window.draw(renderer);
    }

private:
    // Collision boxes for the particles. Need to be stored in the SpatialBuffer
    // dgm::SpatialBuffer<dgm::Circle, std::uint32_t> collisionBoxes;
    dgm::ps::ParticleSystemRenderer renderer;
    dgm::SpatialBuffer<std::unique_ptr<WhiteRectangleParticle>, std::uint32_t>
        particles;
};

struct TextWrapper
{
    sf::Font font;
    sf::Text text;
};

std::unique_ptr<TextWrapper>
createTextWrapper(const std::filesystem::path& fontPath)
{
    auto result = std::make_unique<TextWrapper>();
    if (!result->font.openFromFile(fontPath.string()))
        throw std::runtime_error(
            std::format("Cannot load path {}", fontPath.string()));
    result->text.setFont(result->font);
    result->text.setFillColor(sf::Color::Red);
    result->text.setStyle(sf::Text::Bold);
    return std::move(result);
}

int main(int, char*[])
{
    auto&& window = dgm::Window({ 1280, 720 }, "Sandbox", false);
    dgm::Time time;

    auto&& agentsViz = CollidingAgentsParticleSystem(20000);
    auto&& textWrapper =
        createTextWrapper(RESOURCE_DIR + std::string("/cruft.ttf"));
    auto&& fpsCounter = FpsCounter();

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) std::ignore = window.close();
        }

        // Logic
        time.reset();

        agentsViz.update(time);
        fpsCounter.update(time);
        textWrapper->text.setString(fpsCounter.getString());

        // Draw
        window.beginDraw();

        agentsViz.drawTo(window);
        window.draw(textWrapper->text);

        window.endDraw();
    }

    return 0;
}
