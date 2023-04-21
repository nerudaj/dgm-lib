#include <DGM/classes/Particle.hpp>

using dgm::ps::Particle;

void Particle::setAnimationFrame(const sf::IntRect& frame) noexcept
{
    quad[0].texCoords = sf::Vector2f(float(frame.left), float(frame.top));
    quad[1].texCoords =
        sf::Vector2f(float(frame.left + frame.width), float(frame.top));
    quad[2].texCoords = sf::Vector2f(
        float(frame.left + frame.width), float(frame.top + frame.height));
    quad[3].texCoords =
        sf::Vector2f(float(frame.left), float(frame.top + frame.height));
}

// This just makes a unit vector that has angle of 45°
constexpr float SIN_OF_45_DEG = 0.707106f;
const sf::Vector2f VEC_45_DEG = sf::Vector2f(SIN_OF_45_DEG, -SIN_OF_45_DEG);

void dgm::ps::Particle::setRotation(const float angle) noexcept
{
    rotation = angle;

    // Backup position of the center of the particle
    // and create a unit vector that has been rotated so it points to position
    // of first vertex after rotation and is scaled to point to one of the new
    // vertex positions
    const auto POS = getPosition();
    const auto BASE_VEC =
        dgm::Math::rotateVector(VEC_45_DEG, rotation) * diagonalHalfLength;

    // So the first vertex is just offset from center POS in direction of
    // BASE_VEC Each subsequent vertex is perpendicular to the previous one so
    // we do the same and just alter signs and order of X and Y components
    quad[0].position = POS + sf::Vector2f(BASE_VEC.y, -BASE_VEC.x);
    quad[1].position = POS + BASE_VEC;
    quad[2].position = POS + sf::Vector2f(-BASE_VEC.y, BASE_VEC.x);
    quad[3].position = POS - BASE_VEC;
}

void Particle::spawn(
    const sf::Vector2f& newPosition,
    const sf::Vector2f& newSize,
    const sf::Time& newLifespan)
{
    lifespan = newLifespan.asSeconds();
    const auto halfSize = newSize / 2.f;
    rotation = 0.f;
    diagonalHalfLength = dgm::Math::vectorSize(halfSize);

    sf::Vector2f offsets[] = { { -halfSize.x, -halfSize.y },
                               { halfSize.x, -halfSize.y },
                               { halfSize.x, halfSize.y },
                               { -halfSize.x, halfSize.y } };

    for (int i = 0; i < 4; i++)
    {
        quad[i].position = newPosition + offsets[i];
    }
}
