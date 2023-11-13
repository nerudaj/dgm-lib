#include "DGM/classes/NavMesh.hpp"
#include "DGM/classes/Error.hpp"
#include <array>
#include <cmath>
#include <functional>
#include <map>
#include <queue>

/*
TODO: When CMake is able to set Scan sources for module dependencies, then
import modules: import <queue>; import <array>; import <functional>; import
<map>; import <cmath>;
*/

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

    [[nodiscard]] bool hasElements() const noexcept
    {
        return !nodes.empty();
    }

    [[nodiscard]] bool contains(const sf::Vector2u& p) const noexcept
    {
        const auto itr = nodes.find(p);
        return itr != nodes.end();
    }

    [[nodiscard]] NodeType popBestNode()
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

    [[nodiscard]] const NodeType& getNode(const sf::Vector2u& point) const
    {
        return nodes.at(point);
    }
};

namespace custom
{
    constexpr [[nodiscard]] unsigned max(unsigned a, unsigned b) noexcept
    {
        return a < b ? b : a;
    }

    constexpr [[nodiscard]] unsigned min(unsigned a, unsigned b) noexcept
    {
        return a < b ? a : b;
    }

    constexpr [[nodiscard]] unsigned
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
 *  Returns ClosedSet from which path can be reconstructed or returns empty set
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
    static constexpr [[nodiscard]] unsigned
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

    [[nodiscard]] constexpr bool
    isCloserThan(const TileNode& other) const noexcept
    {
        return fcost < other.fcost
               || (fcost == other.fcost && hcost < other.hcost);
    }
};

dgm::Path<dgm::TileNavpoint> dgm::TileNavMesh::getPath(
    const sf::Vector2u& from, const sf::Vector2u& to, const dgm::Mesh& mesh)
{
    return computePath(from, to, mesh)
        .or_else(
            []() -> std::optional<dgm::Path<dgm::TileNavpoint>>
            { throw dgm::Exception("Path doesn't exist"); })
        .value();
}

std::optional<dgm::Path<dgm::TileNavpoint>> dgm::TileNavMesh::computePath(
    const sf::Vector2u& from, const sf::Vector2u& to, const dgm::Mesh& mesh)
{
    if (from == to) return dgm::Path<TileNavpoint>({}, false);

    auto updateOpenSetWithCoord =
        [&](NodeSet<TileNode>& openSet,
            const sf::Vector2u& coord,
            const NodeSet<TileNode>& closedSet) -> void
    {
        using NeighborType = std::pair<sf::Vector2u, Backdir>;

        auto canVisit = [&](const sf::Vector2u& point)
        {
            const bool notInClosedSet = !closedSet.contains(point);
            const bool emptyInMesh = mesh.at(point.x, point.y) <= 0;
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
        }

        if (point == from) break;
        points.push_back(TileNavpoint(point, 0u));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path(points, false);
}

// ========= WORLD NAVMESH ===========

enum class SeekDir : std::uint8_t
{
    // Upper two bits are used to denote vertical/horizontal direction
    // If both bits are toggled, then its diagonal. It will be useful
    // to reduce code for testing neighbours
    // Lower bits are just iterating unique values for each code
    Up = 0b10000001,
    UpRight = 0b11000010,
    Right = 0b01000011,
    DownRight = 0b11000100,
    Down = 0b10000101,
    DownLeft = 0b11000110,
    Left = 0b01000111,
    UpLeft = 0b11001000
};

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

    [[nodiscard]] bool isCloserThan(const WorldNode& other) const noexcept
    {
        return fcost < other.fcost
               || (fcost == other.fcost && hcost < other.hcost);
    }
};

void dgm::WorldNavMesh::discoverConnectionsForJumpPoint(
    const sf::Vector2u& point, bool fullSearch, bool symmetricConnections)
{
    using Seeker = std::pair<sf::Vector2u, SeekDir>;

    auto isJumpPoint = [&](const sf::Vector2u& p)
    { return jumpPointConnections.find(p) != jumpPointConnections.end(); };

    // point is current seeker point, origin is jump point from which
    // connections are being discovered
    auto isHorizontalNeighborOk =
        [&](const sf::Vector2u& point, const sf::Vector2u& origin)
    {
        // If point is not on the same axis as origin, then we need to check
        // horizontal neighbor that is closer to origin. If it is solid wall
        // then direct visibility to point is compromised and function returns
        // false
        if (point.x < origin.x && mesh.at(point.x + 1, point.y) > 0)
            return false;
        else if (point.x > origin.x && mesh.at(point.x - 1, point.y) > 0)
            return false;
        return true;
    };

    auto isVerticalNeighborOk =
        [&](const sf::Vector2u& point, const sf::Vector2u& origin)
    {
        // Same as test for horizontal neighbour, just for Y axis
        if (point.y < origin.y && mesh.at(point.x, point.y + 1) > 0)
            return false;
        else if (point.y > origin.y && mesh.at(point.x, point.y - 1) > 0)
            return false;
        return true;
    };

    auto isVerticalSeekDir = [&](SeekDir dir)
    { return static_cast<uint8_t>(dir) & 0b10000000; };

    auto isHorizontalSeekDir = [&](SeekDir dir)
    { return static_cast<uint8_t>(dir) & 0b01000000; };

    auto connectTwoJumpPoints =
        [&](const sf::Vector2u& a, const sf::Vector2u& b)
    {
        const float dx =
            (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
        const float dy =
            (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
        const unsigned distance =
            static_cast<unsigned>(std::sqrt(dx * dx + dy * dy));
        jumpPointConnections[a].push_back(Connection(b, distance));

        if (symmetricConnections)
            jumpPointConnections[b].push_back(Connection(a, distance));
    };

    std::queue<Seeker> seekers;

    // For discovery of basic jump points, we only need a partial search since
    // everything is symmetrical
    seekers.push({ sf::Vector2u(point.x + 1, point.y - 1), SeekDir::UpRight });
    seekers.push({ sf::Vector2u(point.x + 1, point.y), SeekDir::Right });
    seekers.push(
        { sf::Vector2u(point.x + 1, point.y + 1), SeekDir::DownRight });
    seekers.push({ sf::Vector2u(point.x, point.y + 1), SeekDir::Down });

    // For source/end points of actual pathfinding, we need full search
    if (fullSearch)
    {
        seekers.push({ sf::Vector2u(point.x, point.y - 1), SeekDir::Up });
        seekers.push(
            { sf::Vector2u(point.x - 1, point.y - 1), SeekDir::UpLeft });
        seekers.push({ sf::Vector2u(point.x - 1, point.y), SeekDir::Left });
        seekers.push(
            { sf::Vector2u(point.x - 1, point.y + 1), SeekDir::DownLeft });
    }

    while (!seekers.empty())
    {
        const auto [coord, dir] = seekers.front();
        seekers.pop();

        // Skip if seeker hit a wall or neighbor compromises visibility
        if (mesh.at(coord) > 0) continue;
        if (isVerticalSeekDir(dir) && !isVerticalNeighborOk(coord, point))
            continue;
        if (isHorizontalSeekDir(dir) && !isHorizontalNeighborOk(coord, point))
            continue;

        // If coord is valid jump point, do not continue search from there
        if (isJumpPoint(coord))
        {
            connectTwoJumpPoints(point, coord);
            continue;
        }

        // Seeker didn't find anything and visibility not compromised, spawn new
        // seekers
        switch (dir)
        {
        case SeekDir::Up:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), dir });
            break;
        case SeekDir::Left:
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), dir });
            break;
        case SeekDir::Down:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), dir });
            break;
        case SeekDir::Right:
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y), dir });
            break;
        case SeekDir::UpLeft:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), SeekDir::Up });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), SeekDir::Left });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y - 1), dir });
            break;
        case SeekDir::UpRight:
            seekers.push({ sf::Vector2u(coord.x, coord.y - 1), SeekDir::Up });
            seekers.push(
                { sf::Vector2u(coord.x + 1, coord.y), SeekDir::Right });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y - 1), dir });
            break;
        case SeekDir::DownLeft:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), SeekDir::Down });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y), SeekDir::Left });
            seekers.push({ sf::Vector2u(coord.x - 1, coord.y + 1), dir });
            break;
        case SeekDir::DownRight:
            seekers.push({ sf::Vector2u(coord.x, coord.y + 1), SeekDir::Down });
            seekers.push(
                { sf::Vector2u(coord.x + 1, coord.y), SeekDir::Right });
            seekers.push({ sf::Vector2u(coord.x + 1, coord.y + 1), dir });
            break;
        }
    }
}

dgm::Path<dgm::WorldNavpoint>
dgm::WorldNavMesh::getPath(const sf::Vector2f& from, const sf::Vector2f& to)
{
    return computePath(from, to)
        .or_else(
            []() -> std::optional<dgm::Path<dgm::WorldNavpoint>>
            { throw dgm::Exception("No path was found"); })
        .value();
}

std::optional<dgm::Path<dgm::WorldNavpoint>>
dgm::WorldNavMesh::computePath(const sf::Vector2f& from, const sf::Vector2f& to)
{
    // Compute tile coordinates of start and end
    const sf::Vector2u tileFrom = sf::Vector2u(
        static_cast<unsigned>(from.x) / mesh.getVoxelSize().x,
        static_cast<unsigned>(from.y) / mesh.getVoxelSize().y);
    const sf::Vector2u tileTo = sf::Vector2u(
        static_cast<unsigned>(to.x) / mesh.getVoxelSize().x,
        static_cast<unsigned>(to.y) / mesh.getVoxelSize().y);

    const bool fromIsJumpPoint = jumpPointConnections.contains(tileFrom);
    const bool toIsJumpPoint = jumpPointConnections.contains(tileTo);

    if (tileFrom == tileTo)
        return dgm::Path<WorldNavpoint>({}, false);
    else if (mesh.at(tileTo) >= 1)
        return std::nullopt;

    // Make auxiliary connections to rest of network, unless source/target point
    // is already a jump point
    if (not toIsJumpPoint)
        discoverConnectionsForJumpPoint(
            tileTo, true, true); // First evaluate destination so start point
                                 // can find direct connection
    if (not fromIsJumpPoint)
        discoverConnectionsForJumpPoint(
            tileFrom, true, false); // do not make symmetrical connections so
                                    // the erasure is constant

    // A*
    auto updateOpenSetWithCoord =
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

    auto closedSet = astarSearch<WorldNode>(
        WorldNode(tileFrom, {}, tileTo, 0), tileTo, updateOpenSetWithCoord);

    {
        // Erase auxiliary connections created earlier, but only if from/to
        // points weren't jump points to begin with
        if (not toIsJumpPoint)
        {
            for (auto&& conn : jumpPointConnections[tileTo])
                jumpPointConnections[conn.destination].pop_back();
            jumpPointConnections.erase(tileTo);
        }

        // source is connected in one-way only, sufficient to delete this node
        if (not fromIsJumpPoint) jumpPointConnections.erase(tileFrom);
    }

    if (!closedSet.hasElements()) return std::nullopt;

    auto getWorldNavpoint = [&](const sf::Vector2u& coord)
    {
        return WorldNavpoint(
            sf::Vector2f(
                (coord.x + 0.5f) * mesh.getVoxelSize().x,
                (coord.y + 0.5f) * mesh.getVoxelSize().y),
            0u);
    };

    std::vector<WorldNavpoint> points;
    points.push_back(getWorldNavpoint(tileTo));

    sf::Vector2u point = tileTo;
    do
    {
        const auto node = closedSet.getNode(point);
        point = node.predecessor;
        if (point == tileFrom) break;
        points.push_back(getWorldNavpoint(point));
    } while (true);
    std::reverse(points.begin(), points.end());

    return dgm::Path<WorldNavpoint>(points, false);
}

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

        const bool northOpened = mesh.at(point.x, point.y - 1) <= 0;
        const bool westOpened = mesh.at(point.x - 1, point.y) <= 0;
        const bool southOpened = mesh.at(point.x, point.y + 1) <= 0;
        const bool eastOpened = mesh.at(point.x + 1, point.y) <= 0;
        const bool northWestCorner =
            mesh.at(point.x - 1, point.y - 1) > 0 && westOpened && northOpened;
        const bool northEastCorner =
            mesh.at(point.x + 1, point.y - 1) > 0 && eastOpened && northOpened;
        const bool southWestCorner =
            mesh.at(point.x - 1, point.y + 1) > 0 && westOpened && southOpened;
        const bool southEastCorner =
            mesh.at(point.x + 1, point.y + 1) > 0 && eastOpened && southOpened;

        return northWestCorner || northEastCorner || southWestCorner
               || southEastCorner;
    };

    auto discoverJumpPoints = [&]()
    {
        for (unsigned y = 1; y < mesh.getDataSize().y - 1; y++)
        {
            for (unsigned x = 1; x < mesh.getDataSize().x - 1; x++)
            {
                // Skip impassable blocks
                if (mesh.at(x, y) > 0) continue;

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
