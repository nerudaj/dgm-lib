#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Path.hpp>
#include <DGM/classes/Utility.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>

namespace std
{
    template<>
    struct hash<sf::Vector2u>
    {
        std::size_t operator()(const sf::Vector2u& vec) const
        {
            using Unsigned = decltype(vec.x);
            if constexpr (sizeof(Unsigned) * 2 <= sizeof(std::size_t))
            {
                return (static_cast<std::size_t>(vec.x)
                        << (sizeof(Unsigned) * 8))
                       + vec.y;
            }
            else
            {
                return vec.x ^ vec.y;
            }
        }
    };
}; // namespace std

namespace dgm
{
    /**
     *  \brief Class for computing path from collision mesh data
     *
     *  This version of NavMesh computes sequence of tile indices
     *  to get from tile A to tile B. Tiles are direct horizontal
     *  or vertical neigbors, diagonal neighbors are ignored.
     *
     *  Path is determined with regard to impassable tiles in dgm::Mesh
     * provided. If tile in mesh has value <= 0 then it is passible. Any value
     * greater than zero is considered impassable.
     */
    class TileNavMesh
    {
    public:
        TileNavMesh() = delete;
        TileNavMesh(const TileNavMesh&) = delete;
        TileNavMesh(TileNavMesh&&) = delete;

        /**
         *  \brief Get path represented by tile indices to input mesh
         *
         *  The resulting path will not include 'from' coord, but it includes
         * 'to' coord.
         *
         *  If no path exists, empty optional will be returned
         *  If from == to, then empty path (which returns true for isTraversed
         * and has zero length) is returned
         */
        static NODISCARD_RESULT std::optional<dgm::Path<TileNavpoint>>
        computePath(
            const sf::Vector2u& from,
            const sf::Vector2u& to,
            const dgm::Mesh& mesh);
    };

    /**
     *  Class for computing path from collision mesh data
     *
     *  This version of NavMesh produces paths with only few key
     *  "jump points". Those jump points are world coordinates placed
     *  in the center of key tiles.
     *
     *  Jump points are picked is a way that they allow for diagonal
     *  movement (thus they produce shorter and more natural paths than
     * TileNavMesh) and there is no obstacle (impassable tile) between two
     * subsequent ones.
     *
     *  Example:
     *  S 1 . .
     *  # . . .
     *  # . . E
     *
     *  S is start, 1 is jump point, E is end. Your path will contain sequence
     * [1, E].
     *
     *  Example 2:
     *  S 1 . 3
     *  # . # .
     *  # 2 . E
     *
     *  In this case, path would contain sequence [1, 2, E] or [1, 3, E]
     *
     *  Example 3:
     *  S 1 # .
     *  # 2 . 3
     *  # . . E
     *
     *  In this case, there is (partially) obstacle on direct route from 1 to E.
     *  However, there is clear path from 2 to E. Thus, path will be sequence
     * [1, 2, E]. Point 3 would be ignored in this case.
     *
     *  This class makes a copy of source mesh because it needs to do some
     * pre-processing before it can do pathfinding. As long as your mesh data
     * don't change, you can reuse object of this class.
     */
    class [[nodiscard]] WorldNavMesh
    {
    public:
        WorldNavMesh() = delete;
        explicit WorldNavMesh(const dgm::Mesh& mesh);
        WorldNavMesh(WorldNavMesh&& other) = default;
        WorldNavMesh(const WorldNavMesh& other) = delete;

    public:
        /**
         *  \brief Get path represented by world coordinates
         *
         *  The resulting path will not include 'from' coord, but it includes
         * 'to' coord.
         *
         *  If no path exists, empty optional is returned
         *  If from == to, then empty path (which returns true for isTraversed)
         * is returned
         *
         *  \warn This function is not thread-safe.
         */
        NODISCARD_RESULT std::optional<dgm::Path<WorldNavpoint>>
        computePath(const sf::Vector2f& from, const sf::Vector2f& to);

    protected:
        struct [[nodiscard]] Connection final
        {
            sf::Vector2u destination; ///< Destination node of the connection
            unsigned distance;        ///< Distance to destination
        };

    protected:
        dgm::Mesh mesh;

        /**
         *  \brief Map of connections between jump points
         *
         *  Map is indexed by jump point coordinates and each jump point lists
         * set of jump points it is connected to, alonside with distance to each
         * of them.
         *
         *  Jump points are only relevant for computation of WorldNavpoints
         */
        std::unordered_map<sf::Vector2u, std::vector<Connection>>
            jumpPointConnections = {};

    protected:
        NODISCARD_RESULT bool isJumpPoint(const sf::Vector2u& p) const
        {
            return jumpPointConnections.contains(p);
        }

        void discoverConnectionsForJumpPoint(
            const sf::Vector2u& point, bool symmetricConnection = false);

        void connectTwoJumpPoints(
            const sf::Vector2u& a,
            const sf::Vector2u& b,
            bool symmetricConnection);

        void connectToAndFromPointsToTheNetwork(
            const sf::Vector2u& tileFrom, const sf::Vector2u& tileTo);

        void eraseFromAndToPointsFromTheNetwork(
            const sf::Vector2u& tileFrom,
            bool wasTileFromOriginallyJumpPoint,
            const sf::Vector2u& tileTo,
            bool wasTileToOriginallyJumpPoint);

        NODISCARD_RESULT sf::Vector2u toTileCoord(const sf::Vector2f& coord);

        NODISCARD_RESULT WorldNavpoint
        toWorldNavpoint(const sf::Vector2u& coord);
    };

} // namespace dgm
