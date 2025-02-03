#pragma once

class [[nodiscard]] WhiteRectangleParticle final : public dgm::ps::Particle
{
    using super = dgm::ps::Particle;

public:
    [[nodiscard]] explicit WhiteRectangleParticle(sf::Vertex* vertices) noexcept
        : super(vertices)
    {
    }

    void spawn(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        const sf::Time& lifespanL = sf::Time::Zero) override
    {
        super::spawn(position, size, lifespanL);
        body.setRadius(size.x / 2.f);
        body.setPosition(position);
    }

    void moveForwardBy(const sf::Vector2f& fwd) noexcept override
    {
        super::moveForwardBy(fwd);
        body.move(fwd);
    }

public:
    dgm::Circle body = dgm::Circle({ 0.f, 0.f }, 0.f);
};
