#include <DGM/classes/ParticleSystemRenderer.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

void dgm::ps::ParticleSystemRenderer::draw(
    sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = texture;

    // draw the vertex array
    target.draw(vertices, states);
}
