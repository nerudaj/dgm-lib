#include <DGM/classes/Math.hpp>
#include <DGM/classes/Particle.hpp>
#include <VBOHelper.hpp>

using dgm::ps::Particle;

void Particle::setAnimationFrame(const sf::IntRect& frame) noexcept
{
    VBOHelper::setTextureCoords(vertices.data(), frame);
}

// This just makes a unit vector that has angle of 45°
constexpr float SIN_OF_45_DEG = 0.707106f;
const sf::Vector2f VEC_45_DEG = sf::Vector2f(SIN_OF_45_DEG, -SIN_OF_45_DEG);

void dgm::ps::Particle::setRotation(const sf::Angle angle) noexcept
{
    rotation = angle;

    // Backup position of the center of the particle
    // and create a unit vector that has been rotated so it points to position
    // of first vertex after rotation and is scaled to point to one of the new
    // vertex positions
    const auto POS = getPosition();
    const auto BASE_VEC = VEC_45_DEG.rotatedBy(rotation) * diagonalHalfLength;

    // So the first vertex is just offset from center POS in direction of
    // BASE_VEC Each subsequent vertex is perpendicular to the previous one so
    // we do the same and just alter signs and order of X and Y components
    vertices[0].position = POS + sf::Vector2f(BASE_VEC.y, -BASE_VEC.x);
    vertices[1].position = POS + BASE_VEC;
    vertices[2].position = POS - BASE_VEC;

    vertices[3].position = POS + BASE_VEC;
    vertices[4].position = POS + sf::Vector2f(-BASE_VEC.y, BASE_VEC.x);
    vertices[5].position = POS - BASE_VEC;
}

void Particle::spawn(
    const sf::Vector2f& newPosition,
    const sf::Vector2f& newSize,
    const sf::Time& newLifespan)
{
    lifespan = newLifespan.asSeconds();
    const auto halfSize = newSize / 2.f;
    rotation = sf::Angle::Zero;
    diagonalHalfLength = halfSize.length();

    const sf::Vector2f offsets[] = {
        // Upper-left triangle
        { -halfSize.x, -halfSize.y },
        { halfSize.x, -halfSize.y },
        { -halfSize.x, halfSize.y },
        // Bottom-right triangle
        { halfSize.x, -halfSize.y },
        { halfSize.x, halfSize.y },
        { -halfSize.x, halfSize.y },
    };

    assert(vertices.size() == std::size(offsets));
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].position = newPosition + offsets[i];
    }
}
