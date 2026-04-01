#include <DGM/classes/Raycaster.hpp>
#include <cmath>

bool dgm::Raycaster::hasDirectVisibility(
    const sf::Vector2f& from,
    const sf::Vector2f& to,
    const dgm::Mesh& levelMesh)
{
    const auto normalizedFrom =
        from.componentWiseDiv(sf::Vector2f(levelMesh.getVoxelSize()));
    const auto normalizedTo =
        to.componentWiseDiv(sf::Vector2f(levelMesh.getVoxelSize()));

    auto&& state = initializeRaycaster(normalizedFrom, to - from);

    HitDirection advancementDirection {};
    while (true)
    {
        if (levelMesh[state.tile] > 0) break;

        advancementDirection = advanceRaycaster(state);
    }

    return (normalizedTo - normalizedFrom).length()
           < getInterceptDistance(state, advancementDirection);
}

dgm::Raycaster::Result dgm::Raycaster::raycast(
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const dgm::Mesh& levelMesh,
    std::function<void(const sf::Vector2u&)> forEachTileCallback)
{
    auto&& normalizedFrom =
        origin.componentWiseDiv(sf::Vector2f(levelMesh.getVoxelSize()));
    auto&& state = initializeRaycaster(normalizedFrom, direction);

    HitDirection advancementDirection {};
    while (true)
    {
        if (levelMesh[state.tile] > 0) break;

        forEachTileCallback(state.tile);
        advancementDirection = advanceRaycaster(state);
    }

    return Result {
        .tile = state.tile,
        .hitDirection = advancementDirection,
        // TODO: hitPosition
    };
}

dgm::Raycaster::Result dgm::Raycaster::raycast(
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const dgm::Mesh& levelMesh)
{
    auto&& normalizedFrom =
        origin.componentWiseDiv(sf::Vector2f(levelMesh.getVoxelSize()));
    auto&& state = initializeRaycaster(normalizedFrom, direction);

    HitDirection advancementDirection {};
    while (true)
    {
        if (levelMesh[state.tile] > 0) break;

        advancementDirection = advanceRaycaster(state);
    }

    return Result {
        .tile = state.tile,
        .hitDirection = advancementDirection,
        // TODO: hitPosition
    };
}

dgm::Raycaster::RaycasterState dgm::Raycaster::initializeRaycaster(
    const sf::Vector2f& from, const sf::Vector2f& direction)
{
    auto tile = sf::Vector2u(from);
    auto tileStep = sf::Vector2i(0, 0);
    auto rayStep = sf::Vector2f(
        std::sqrt(
            1 + (direction.y * direction.y) / (direction.x * direction.x)),
        std::sqrt(
            1 + (direction.x * direction.x) / (direction.y * direction.y)));
    auto&& intercept = sf::Vector2f();

    if (direction.x < 0)
    {
        tileStep.x = -1;
        intercept.x = (from.x - tile.x) * rayStep.x;
    }
    else
    {
        tileStep.x = 1;
        intercept.x = (tile.x + 1 - from.x) * rayStep.x;
    }

    if (direction.y < 0)
    {
        tileStep.y = -1;
        intercept.y = (from.y - tile.y) * rayStep.y;
    }
    else
    {
        tileStep.y = 1;
        intercept.y = (tile.y + 1 - from.y) * rayStep.y;
    }

    return RaycasterState {
        .tile = tile,
        .tileStep = tileStep,
        .rayStep = rayStep,
        .intercept = intercept,
    };
}

dgm::Raycaster::HitDirection
dgm::Raycaster::advanceRaycaster(RaycasterState& state)
{
    if (state.intercept.x < state.intercept.y)
    {
        state.tile.x += state.tileStep.x;
        state.intercept.x += state.rayStep.x;
        return HitDirection::Vertical;
    }

    state.tile.y += state.tileStep.y;
    state.intercept.y += state.rayStep.y;
    return HitDirection::Horizontal;
}
