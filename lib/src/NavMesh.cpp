#include "DGM/classes/NavMesh.hpp"
#include "DGM/classes/Error.hpp"
#include <JumpPointSearchUtilities.hpp>
#include <array>
#include <cmath>
#include <functional>
#include <map>
#include <queue>

template<class T>
concept AstarNode =
    std::is_class<T>::value
    && std::is_member_function_pointer_v<decltype(&T::isCloserThan)>
    && std::is_member_object_pointer_v<decltype(&T::point)>
    && std::is_member_object_pointer_v<decltype(&T::gcost)>;

template<AstarNode NodeType>
class NodeSet
{
protected:
    std::map<sf::Vector2u, NodeType, dgm::Utility::less<sf::Vector2u>> nodes;

public:
    void insertNode(const NodeType& node)
    {
        if (nodes.find(node.point) != nodes.end()
            && nodes[node.point].gcost < node.gcost)
            return;
        nodes[node.point] = node;
    }

    NODISCARD_RESULT bool hasElements() const noexcept
    {
        return !nodes.empty();
    }

    NODISCARD_RESULT bool contains(const sf::Vector2u& p) const noexcept
    {
        const auto itr = nodes.find(p);
        return itr != nodes.end();
    }

    NODISCARD_RESULT NodeType popBestNode()
    {
        auto minElem = nodes.begin();
        for (auto itr = (++nodes.begin()); itr != nodes.end(); itr++)
        {
            if (itr->second.isCloserThan(minElem->second))
            {
                minElem = itr;
            }
        }

        const NodeType copy = minElem->second;
        nodes.erase(minElem);
        return copy;
    }

    NODISCARD_RESULT const NodeType& getNode(const sf::Vector2u& point) const
    {
        return nodes.at(point);
    }
};

namespace custom
{
    constexpr NODISCARD_RESULT unsigned max(unsigned a, unsigned b) noexcept
    {
        return a < b ? b : a;
    }

    constexpr NODISCARD_RESULT unsigned min(unsigned a, unsigned b) noexcept
    {
        return a < b ? a : b;
    }

    constexpr NODISCARD_RESULT unsigned
    getScalarDistance(unsigned a, unsigned b) noexcept
    {
        return max(a, b) - min(a, b);
    }
} // namespace custom

template<AstarNode NodeType>
// First parameter is open set, second is coordinate to update from and third
// parameter is closed set
using UpdateOpenSetWithCoordCallback = std::function<void(
    NodeSet<NodeType>&, const sf::Vector2u&, const NodeSet<NodeType>&)>;

/*
 * Returns ClosedSet from which path can be reconstructed or returns empty set
 * if no path was found
 */
template<AstarNode NodeType>
static decltype(auto) astarSearch(
    NodeType startNode,
    const sf::Vector2u& destinationCoord,
    UpdateOpenSetWithCoordCallback<NodeType> updateOpenSetWithCoordCallback)
{
    NodeSet<NodeType> openSet, closedSet;

    closedSet.insertNode(startNode);
    updateOpenSetWithCoordCallback(openSet, startNode.point, closedSet);

    while (openSet.hasElements())
    {
        const NodeType node = openSet.popBestNode();
        closedSet.insertNode(node);

        if (node.point == destinationCoord) return closedSet;

        updateOpenSetWithCoordCallback(openSet, node.point, closedSet);
    }

    return NodeSet<NodeType>();
}

enum class Backdir
{
    Undefined,
    Up,
    Left,
    Down,
    Right
};

struct TileNode
{
protected:
    static CONSTEXPR_NODISCARD unsigned
    getManhattanDistance(const sf::Vector2u& a, const sf::Vector2u& b) noexcept
    {
        return custom::getScalarDistance(a.x, b.x)
               + custom::getScalarDistance(a.y, b.y);
    }

public:
    sf::Vector2u point = {};
    unsigned gcost = 0;
    unsigned hcost = 0;
    unsigned fcost = 0;
    Backdir backdir = Backdir::Up;

    TileNode() = default;

    TileNode(
        const sf::Vector2u& point,
        const sf::Vector2u& end,
        unsigned gcost,
        Backdir backdir)
    {
        this->point = point;
        this->backdir = backdir;
        this->gcost = gcost;
        hcost = getManhattanDistance(point, end);
        fcost = gcost + hcost;
    }

    TileNode(const TileNode& other) = default;

    NODISCARD_RESULT constexpr bool
    isCloserThan(const TileNode& other) const noexcept
    {
        return fcost < other.fcost
               || (fcost == other.fcost && hcost < other.hcost);
    }
};

std::optional<dgm::Path<dgm::TileNavpoint>> dgm::TileNavMesh::computePath(
    const sf::Vector2u& from, const sf::Vector2u& to, const dgm::Mesh& mesh)
{
    if (mesh[from] == 1)
        return std::nullopt;
    else if (from == to)
        return dgm::Path<TileNavpoint>({}, false);

    auto updateOpenSetWithCoord =
        [&](NodeSet<TileNode>& openSet,
            const sf::Vector2u& coord,
            const NodeSet<TileNode>& closedSet) -> void
    {
        using NeighborType = std::pair<sf::Vector2u, Backdir>;

        auto canVisit = [&](const sf::Vector2u& point)
        {
            const bool notInClosedSet = !closedSet.contains(point);
            const bool emptyInMesh = mesh[point] <= 0;
            return notInClosedSet && emptyInMesh;
        };

        const TileNode& node = closedSet.getNode(coord);

        // Direction points from Vector2u to node.point
        const std::array<NeighborType, 4> neighbors = {
            NeighborType(
                sf::Vector2u(node.point.x, node.point.y - 1), Backdir::Down),
            NeighborType(
                sf::Vector2u(node.point.x, node.point.y + 1), Backdir::Up),
            NeighborType(
                sf::Vector2u(node.point.x - 1, node.point.y), Backdir::Right),
            NeighborType(
                sf::Vector2u(node.point.x + 1, node.point.y), Backdir::Left)
        };

        for (auto&& [newCoord, backdir] : neighbors)
        {
            if (canVisit(newCoord))
                openSet.insertNode(
                    TileNode(newCoord, to, node.gcost + 1, backdir));
        }
    };

    const NodeSet<TileNode> closedSet = astarSearch<TileNode>(
        TileNode(from, to, 0, Backdir::Undefined), to, updateOpenSetWithCoord);

    if (!closedSet.hasElements()) return std::nullopt;

    std::vector<TileNavpoint> points;
    points.push_back(TileNavpoint(to, 0u));

    sf::Vector2u point = to;
    do
    {
        const TileNode& node = closedSet.getNode(point);
        switch (node.backdir)
        {
        case Backdir::Up:
            --point.y;
            break;
        case Backdir::Left:
            --point.x;
            break;
        case Backdir::Down:
            ++point.y;
            break;
        case Backdir::Right:
            ++point.x;
            break;
        case Backdir::Undefined:
            break;
        }

        if (point == from) break;
        points.push_back(TileNavpoint(point, 0u));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path(points, false);
}

// ========= WORLD NAVMESH ===========

struct WorldNode
{
    sf::Vector2u point = {};
    sf::Vector2u predecessor = {};
    unsigned gcost = 0;
    unsigned hcost = 0;
    unsigned fcost = 0;

    WorldNode() = default;

    WorldNode(
        const sf::Vector2u& point,
        const sf::Vector2u& pred,
        const sf::Vector2u& end,
        unsigned gcost)
    {
        this->point = point;
        predecessor = pred;
        this->gcost = gcost;
        const unsigned dx = (point.x - end.x);
        const unsigned dy = (point.y - end.y);
        hcost = static_cast<unsigned>(
            std::sqrt(static_cast<float>(dx * dx + dy * dy)));
        fcost = gcost + hcost;
    }

    WorldNode(const WorldNode& other) = default;

    NODISCARD_RESULT constexpr bool
    isCloserThan(const WorldNode& other) const noexcept
    {
        return fcost < other.fcost
               || (fcost == other.fcost && hcost < other.hcost);
    }
};

dgm::WorldNavMesh::WorldNavMesh(const dgm::Mesh& mesh) : mesh(mesh)
{
    auto isJumpPoint = [&](const sf::Vector2u& point)
    {
        /**
         *  Test if this point is at the tip of some impassable tile, eg:
         *  #   #
         *  # p     <-- there the p is corner point
         *  # # #
         *
         *
         *  #   #
         *  # p #   <-- there p is not a corner point
         *  #   #
         *
         *  #   #
         *    p     <-- multiple corners
         *  # # #
         */

        const bool northOpened = mesh[{ point.x, point.y - 1 }] <= 0;
        const bool westOpened = mesh[{ point.x - 1, point.y }] <= 0;
        const bool southOpened = mesh[{ point.x, point.y + 1 }] <= 0;
        const bool eastOpened = mesh[{ point.x + 1, point.y }] <= 0;
        const bool northWestCorner =
            mesh[{ point.x - 1, point.y - 1 }] > 0 && westOpened && northOpened;
        const bool northEastCorner =
            mesh[{ point.x + 1, point.y - 1 }] > 0 && eastOpened && northOpened;
        const bool southWestCorner =
            mesh[{ point.x - 1, point.y + 1 }] > 0 && westOpened && southOpened;
        const bool southEastCorner =
            mesh[{ point.x + 1, point.y + 1 }] > 0 && eastOpened && southOpened;

        return northWestCorner || northEastCorner || southWestCorner
               || southEastCorner;
    };

    auto discoverJumpPoints = [&]
    {
        for (unsigned y = 1; y < mesh.getDataSize().y - 1; y++)
        {
            for (unsigned x = 1; x < mesh.getDataSize().x - 1; x++)
            {
                // Skip impassable blocks
                if (mesh[{ x, y }] > 0) continue;

                sf::Vector2u point(x, y);
                if (isJumpPoint(point))
                {
                    jumpPointConnections[point] = {};
                }
            }
        }
    };

    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point, false);
}

static std::optional<dgm::Path<dgm::WorldNavpoint>> convertRawPathToWorldPath(
    const NodeSet<WorldNode>& rawPath,
    const sf::Vector2u& tileFrom,
    const sf::Vector2u& tileTo,
    std::function<dgm::WorldNavpoint(const sf::Vector2u&)> toWorldNavpoint)
{
    if (!rawPath.hasElements()) return std::nullopt;

    std::vector<dgm::WorldNavpoint> points;
    points.push_back(toWorldNavpoint(tileTo));

    sf::Vector2u point = tileTo;
    do
    {
        const auto node = rawPath.getNode(point);
        point = node.predecessor;
        if (point == tileFrom) break;
        points.push_back(toWorldNavpoint(point));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path<dgm::WorldNavpoint>(points, false);
}

std::optional<dgm::Path<dgm::WorldNavpoint>>
dgm::WorldNavMesh::computePath(const sf::Vector2f& from, const sf::Vector2f& to)
{
    const auto&& tileFrom = toTileCoord(from);
    const auto&& tileTo = toTileCoord(to);
    const auto&& wasTileFromOriginallyJumpPoint = isJumpPoint(tileFrom);
    const auto&& wasTileToOriginallyJumpPoint = isJumpPoint(tileTo);

    // Early search pruning
    if (tileFrom == tileTo) // Identity
        return dgm::Path<WorldNavpoint>({}, false);
    else if (mesh[tileTo] > 0) // Destination is a wall
        return std::nullopt;

    connectToAndFromPointsToTheNetwork(tileFrom, tileTo);

    // A*
    auto&& insertCoordIntoOpenSetIfNotInClosedSetAlready =
        [&](NodeSet<WorldNode>& openSet,
            const sf::Vector2u& coord,
            const NodeSet<WorldNode>& closedSet) -> void
    {
        const auto node = closedSet.getNode(coord);
        for (auto&& conn : jumpPointConnections[coord])
        {
            if (!closedSet.contains(
                    conn.destination)) // Do not re-evaluate node which we've
                                       // already visited on faster route
                openSet.insertNode(WorldNode(
                    conn.destination,
                    coord,
                    tileTo,
                    node.gcost + static_cast<unsigned>(conn.distance)));
        }
    };

    auto&& rawPath = astarSearch<WorldNode>(
        WorldNode(tileFrom, {}, tileTo, 0),
        tileTo,
        insertCoordIntoOpenSetIfNotInClosedSetAlready);

    eraseFromAndToPointsFromTheNetwork(
        tileFrom,
        wasTileFromOriginallyJumpPoint,
        tileTo,
        wasTileToOriginallyJumpPoint);

    return convertRawPathToWorldPath(
        rawPath,
        tileFrom,
        tileTo,
        std::bind(
            &dgm::WorldNavMesh::toWorldNavpoint, this, std::placeholders::_1));
}

void dgm::WorldNavMesh::discoverConnectionsForJumpPoint(
    const sf::Vector2u& point, bool symmetricConnection)
{
    using namespace dgm::priv;

    auto&& discoverConnectionsInDirection =
        [&](sf::Vector2u seeker,
            std::function<sf::Vector2u(const sf::Vector2u&)> advance,
            std::function<bool(const sf::Vector2u, const dgm::Mesh&)>
                shouldStopAdvancing)
    {
        while (true)
        {
            if (shouldStopAdvancing(seeker, mesh))
                return seeker;
            else if (isJumpPoint(seeker))
            {
                connectTwoJumpPoints(point, seeker, symmetricConnection);
                return seeker;
            }
            seeker = advance(seeker);
        }
    };

    // Discover vertical connections
    auto&& ystop1 = discoverConnectionsInDirection(
        advanceUp(point), advanceUp, shouldStopStraightDiscovery);
    auto&& ystop2 = discoverConnectionsInDirection(
        advanceDown(point), advanceDown, shouldStopStraightDiscovery);

    // Discover horizontal connections
    auto&& xstop1 = discoverConnectionsInDirection(
        advanceLeft(point), advanceLeft, shouldStopStraightDiscovery);
    auto&& xstop2 = discoverConnectionsInDirection(
        advanceRight(point), advanceRight, shouldStopStraightDiscovery);

    for (unsigned y = point.y - 1; y > ystop1.y; --y)
    {
        discoverConnectionsInDirection(
            { point.x - 1, y }, advanceUpLeft, shouldStopUpLeftDiscovery);
        discoverConnectionsInDirection(
            { point.x + 1, y }, advanceUpRight, shouldStopUpRightDiscovery);
    }

    for (unsigned y = point.y + 1; y < ystop2.y; ++y)
    {
        discoverConnectionsInDirection(
            { point.x - 1, y }, advanceDownLeft, shouldStopDownLeftDiscovery);
        discoverConnectionsInDirection(
            { point.x + 1, y }, advanceDownRight, shouldStopDownRightDiscovery);
    }

    // starting at point.x - 1 would duplicate diagonal ray
    for (unsigned x = point.x - 2; point.x > 1 && x > xstop1.x; --x)
    {
        discoverConnectionsInDirection(
            { x, point.y - 1 }, advanceUpLeft, shouldStopUpLeftDiscovery);
        discoverConnectionsInDirection(
            { x, point.y + 1 }, advanceDownLeft, shouldStopDownLeftDiscovery);
    }

    // starting at point.x + 1 would duplicate diagonal ray
    for (unsigned x = point.x + 2;
         point.x < mesh.getDataSize().x - 1 && x < xstop2.x;
         ++x)
    {
        discoverConnectionsInDirection(
            { x, point.y - 1 }, advanceUpRight, shouldStopUpRightDiscovery);
        discoverConnectionsInDirection(
            { x, point.y + 1 }, advanceDownRight, shouldStopDownRightDiscovery);
    }
}

void dgm::WorldNavMesh::connectTwoJumpPoints(
    const sf::Vector2u& a, const sf::Vector2u& b, bool symmetricConnection)
{
    const float dx = (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
    const float dy = (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
    const unsigned distance =
        static_cast<unsigned>(std::sqrt(dx * dx + dy * dy));

    if (symmetricConnection)
        jumpPointConnections[b].push_back(Connection(a, distance));
    jumpPointConnections[a].push_back(Connection(b, distance));
}

void dgm::WorldNavMesh::connectToAndFromPointsToTheNetwork(
    const sf::Vector2u& tileFrom, const sf::Vector2u& tileTo)
{
    // Unless these points are jump points, connect them to the network
    if (!isJumpPoint(tileTo))
    {
        // Destination needs to have connections from neighbors to itself
        // reverse connections are only made for bookmarking so everything
        // can be cleaned-up later
        jumpPointConnections[tileTo] = {};
        discoverConnectionsForJumpPoint(tileTo, true);
    }

    if (!isJumpPoint(tileFrom))
    {
        // At this point, destination is connected to the netwrok
        // discovery at this point will plug the starting point to
        // the network and with a little luck, it might find a
        // direct path to the destination
        discoverConnectionsForJumpPoint(tileFrom);
    }
}

void dgm::WorldNavMesh::eraseFromAndToPointsFromTheNetwork(
    const sf::Vector2u& tileFrom,
    bool wasTileFromOriginallyJumpPoint,
    const sf::Vector2u& tileTo,
    bool wasTileToOriginallyJumpPoint)
{
    // Erase auxiliary connections created earlier, but only if from/to
    // points weren't jump points to begin with
    if (not wasTileToOriginallyJumpPoint)
    {
        for (auto&& conn : jumpPointConnections[tileTo])
            jumpPointConnections[conn.destination].pop_back();
        jumpPointConnections.erase(tileTo);
    }

    // source is connected in one-way only, sufficient to delete this node
    if (not wasTileFromOriginallyJumpPoint)
        jumpPointConnections.erase(tileFrom);
}

sf::Vector2u dgm::WorldNavMesh::toTileCoord(const sf::Vector2f& coord)
{
    return sf::Vector2u(
        static_cast<unsigned>(coord.x) / mesh.getVoxelSize().x,
        static_cast<unsigned>(coord.y) / mesh.getVoxelSize().y);
}

dgm::WorldNavpoint dgm::WorldNavMesh::toWorldNavpoint(const sf::Vector2u& coord)
{
    return WorldNavpoint(
        sf::Vector2f(
            (coord.x + 0.5f) * mesh.getVoxelSize().x,
            (coord.y + 0.5f) * mesh.getVoxelSize().y),
        0u);
}
