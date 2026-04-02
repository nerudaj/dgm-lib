#pragma once

#include <DGM/classes/Objects.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace dgm
{

    /**
     * @brief Grid-based raycaster using the DDA (Digital Differential
     * Analyzer) algorithm.
     *
     * Provides line-of-sight checks and raycasting against a dgm::Mesh grid.
     * Tiles with a value greater than zero are treated as solid (blocking).
     *
     * All spatial parameters (positions, directions) are specified in world
     * coordinates; they are normalized internally using the mesh voxel size.
     */
    class Raycaster
    {
    public:
        /**
         * @brief Describes which axis boundary the ray crossed when it hit a
         * tile.
         */
        enum class [[nodiscard]] HitDirection : bool
        {
            Vertical,   ///< The ray crossed a vertical (north/south) boundary.
            Horizontal, ///< The ray crossed a horizontal (west/east) boundary.
        };

        /**
         * @brief Result of a raycast operation.
         */
        struct [[nodiscard]] Result final
        {
            sf::Vector2u tile;         ///< Grid coordinates of the hit tile.
            HitDirection hitDirection; ///< Which axis boundary was crossed.
            sf::Vector2f hitLocation;  ///< Precise world-space hit position.
        };

    public:
        /**
         * @brief Check whether there is an unobstructed line of sight between
         * two points.
         *
         * Traces a ray from @p from toward @p to through @p levelMesh and
         * returns @c true if no solid tile is encountered before reaching the
         * destination.
         *
         * @param[in] from       Starting point in world coordinates.
         * @param[in] to         Target point in world coordinates.
         * @param[in] levelMesh  Grid mesh defining the level geometry.
         *                       Tiles with value > 0 are solid.
         * @return @c true if the path is clear, @c false if a solid tile
         *         blocks the line of sight.
         */
        [[nodiscard]] static bool hasDirectVisibility(
            const sf::Vector2f& from,
            const sf::Vector2f& to,
            const dgm::Mesh& levelMesh);

        /**
         * @brief Cast a ray and return the first solid tile it hits.
         *
         * @param[in] origin     Ray origin in world coordinates.
         * @param[in] direction  Ray direction (does not need to be
         *                       normalized).
         * @param[in] levelMesh  Grid mesh defining the level geometry.
         *                       Tiles with value > 0 are solid.
         * @return A Result describing the hit tile, the boundary crossed, and
         *         the hit location.
         */
        static Result raycast(
            const sf::Vector2f& origin,
            const sf::Vector2f& direction,
            const dgm::Mesh& levelMesh);

        /**
         * @brief Cast a ray, invoking a callback for every empty tile visited
         * before a solid tile is hit.
         *
         * @param[in] origin               Ray origin in world coordinates.
         * @param[in] direction            Ray direction (does not need to be
         *                                 normalized).
         * @param[in] levelMesh            Grid mesh defining the level
         *                                 geometry. Tiles with value > 0 are
         *                                 solid.
         * @param[in] forEachTileCallback  Callback invoked with the grid
         *                                 coordinates of each non-solid tile
         *                                 the ray passes through, in order.
         * @return A Result describing the hit tile, the boundary crossed, and
         *         the hit location.
         */
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
