#pragma once

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

namespace dgm
{
    /**
     *  \brief Representation of points used in paths
     *
     *  If float is used for the template, the coordinates of the point are
     * considered to be "world coordinates" If unsigned long is used for the
     * template, the coordinates are considered to be "tile coordinates" within
     * some tilemap
     *
     *  You can use TileNavpoint and WorldNavpoint specializations
     */
    template<class T>
        requires std::is_same<float, T>::value
                 || std::is_same<unsigned, T>::value
    struct [[nodiscard]] Navpoint
    {
        sf::Vector2<T> coord; ///< Coordinate of the point
        uint32_t value = 0;   ///< General purpose value ensuring compatibility
                              ///< with LevelD::Path format
    };

    /**
     *  \brief Navigation point within a tilemap
     */
    using TileNavpoint = Navpoint<unsigned>;

    /**
     *  \brief Navigation point within world
     */
    using WorldNavpoint = Navpoint<float>;

    /**
     *  \brief Class represeting a list of navigation paths to traverse
     *
     *  \note Acceptable template types are TileNavpoint or WorldNavpoint
     *
     *  Path can be looping or non-looping. If it is looping then it can be
     * never traversed as advance will loop back to beginning if last point in
     * list is reached.
     *
     *  This class is constructible either directly or from LevelD::Path.
     */
    template<class T>
        requires std::is_same<TileNavpoint, T>::value
                 || std::is_same<WorldNavpoint, T>::value
    class [[nodiscard]] Path final
    {
    public:
        Path(const std::vector<T>& points, bool looping)
            : points(points), looping(looping)
        {
        }

        Path(dgm::Path<T>&& other) = default;

        ~Path() = default;

        Path<T>& operator=(dgm::Path<T>&& other) = default;

        NODISCARD_RESULT
        Path<T> clone() const
        {
            return Path(*this);
        }

    private:
        explicit Path(const Path&) = default;

    public:
        /**
         *  \brief Test whether there are no more points in the list
         *
         *  \note Path can never be traversed if it is a looping path
         */
        NODISCARD_RESULT constexpr bool isTraversed() const noexcept
        {
            return points.size() <= currentPointIndex;
        }

        NODISCARD_RESULT constexpr bool isLooping() const noexcept
        {
            return looping;
        }

        /**
         *  \brief Get currently processed Navpoint
         *
         *  Once you finish processing this navpoint (i.e.: you reach it)
         *  call advance() to move to next point.
         */
        NODISCARD_RESULT constexpr const T& getCurrentPoint() const noexcept
        {
            assert(not isTraversed());
            return points[currentPointIndex];
        }

        /**
         *  \brief Move processing to next navpoint
         */
        NODISCARD_RESULT constexpr void advance() noexcept
        {
            currentPointIndex++;
            if (isLooping() && isTraversed()) currentPointIndex = 0;
        }

        template<typename = std::enable_if_t<std::is_same_v<T, TileNavpoint>>>
        NODISCARD_RESULT constexpr std::size_t getLength() const noexcept
        {
            return points.size();
        }

    protected:
        std::vector<T> points = {}; ///< List of points to travese
        bool looping = false;       ///< Whether the path loops
        std::size_t currentPointIndex =
            0; ///< Index to currently processed navpoint
    };
} // namespace dgm
