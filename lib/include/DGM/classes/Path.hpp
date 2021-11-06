#pragma once

#include <vector>
#include <concepts>
#include "LevelD.hpp"

namespace dgm {
	/**
	 *  \brief Representation of points used in paths
	 * 
	 *  If float is used for the template, the coordinates of the point are considered to be "world coordinates"
	 *  If unsigned long is used for the template, the coordinates are considered to be "tile coordinates" within some tilemap
	 * 
	 *  You can use TileNavpoint and WorldNavpoint specializations
	 */
	template<class T>
	requires std::is_same<float, T>::value || std::is_same<unsigned long, T>::value
	struct Navpoint {
		T x; ///< X coordinate (world or tile)
		T y; ///< Y coordinate (world or tile)
		uint32_t value = 0; ///< General purpose value ensuring compatibility with LevelD::Path format
	};

	/**
	 *  \brief Navigation point within a tilemap
	 */
	using TileNavpoint = Navpoint<unsigned long>;

	/**
	 *  \brief Navigation point within world
	 */
	using WorldNavpoint = Navpoint<float>;

	/**
	 *  \brief Class represeting a list of navigation paths to traverse
	 * 
	 *  \note Acceptable template types are TileNavpoint or WorldNavpoint
	 * 
	 *  Path can be looping or non-looping. If it is looping then it can be never traversed
	 *  as advance will loop back to beginning if last point in list is reached.
	 * 
	 *  This class is constructible either directly or from LevelD::Path.
	 */
	template<class T>
	requires std::is_same<TileNavpoint, T>::value || std::is_same<WorldNavpoint, T>::value
	class Path {
	protected:
		std::vector<T> points; ///< List of points to travese
		bool looping; ///< Whether the path loops
		std::size_t currentPointIndex; ///< Index to currently processed navpoint

	public:
		/**
		 *  \brief Test whether there are no more points in the list
		 * 
		 *  \note Path can never be traversed if it is a looping path
		 */
		[[nodiscard]] constexpr bool isTraversed() const noexcept;

		/**
		 *  \brief Get currently processed Navpoint
		 * 
		 *  Once you finish processing this navpoint (i.e.: you reach it)
		 *  call advance() to move to next point.
		 */
		[[nodiscard]] const T& getCurrentPoint() const noexcept;

		/**
		 *  \brief Move processing to next navpoint
		 */
		[[nodiscard]] void advance() noexcept;

		Path(const std::vector<T>& points, bool looping);

		Path(const LevelD::Path& lvdPath);
	};
}
