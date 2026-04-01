#pragma once

#include <DGM/classes/Objects.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace dgm
{

    class Raycaster
    {
    public:
        enum class [[nodiscard]] HitDirection : bool
        {
            Vertical,   // north/south
            Horizontal, // west/east
        };

        struct [[nodiscard]] Result final
        {
            sf::Vector2u tile;
            HitDirection hitDirection;
            sf::Vector2f hitLocation;
        };

    public:
        [[nodiscard]] static bool hasDirectVisibility(
            const sf::Vector2f& from,
            const sf::Vector2f& to,
            const dgm::Mesh& levelMesh);

        static Result raycast(
            const sf::Vector2f& origin,
            const sf::Vector2f& direction,
            const dgm::Mesh& levelMesh);

        static Result raycast(
            const sf::Vector2f& origin,
            const sf::Vector2f& direction,
            const dgm::Mesh& levelMesh,
            std::function<void(const sf::Vector2u&)> forEachTileCallback);

    private:
        struct [[nodiscard]] RaycasterState final
        {
            sf::Vector2u tile;
            sf::Vector2i tileStep;
            sf::Vector2f rayStep;
            sf::Vector2f intercept; // precise hit position on tile boundary
        };

    private:
        static RaycasterState initializeRaycaster(
            const sf::Vector2f& from, const sf::Vector2f& direction);

        [[nodiscard]] static HitDirection
        advanceRaycaster(RaycasterState& state);

        [[nodiscard]] static float getInterceptDistance(
            const RaycasterState& state, const HitDirection lastDirection)
        {
            if (lastDirection == HitDirection::Vertical)
                return state.intercept.x - state.rayStep.x;
            else
                return state.intercept.y - state.rayStep.y;
        }
    };

} // namespace dgm
