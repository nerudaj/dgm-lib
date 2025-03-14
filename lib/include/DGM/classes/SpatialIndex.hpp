#pragma once

#include <DGM/classes/Collision.hpp>
#include <DGM/classes/Objects.hpp>
#include <algorithm>
#include <concepts>
#include <functional>
#include <vector>

namespace dgm
{
    /**
     * \brief Concept for type that is recognized by
     * the spatial buffer for AABB collision testing.
     */
    template<class T>
    concept AaBbType =
        std::is_same_v<T, sf::Vector2f> || std::is_same_v<T, dgm::Circle>
        || std::is_same_v<T, dgm::Rect>;

    template<
        typename IndexType = std::size_t,
        typename GridResolutionType = unsigned>
    class [[nodiscard]] SpatialIndex
    {
    public:
        using IndexingType = IndexType;
        using IndexListType = std::vector<IndexType>;

    public:
        constexpr SpatialIndex(
            dgm::Rect boundingBox, GridResolutionType gridResolution)
            : BOUNDING_BOX(std::move(boundingBox))
            , GRID_RESOLUTION(gridResolution)
            , COORD_TO_GRID_X(gridResolution / BOUNDING_BOX.getSize().x)
            , COORD_TO_GRID_Y(gridResolution / BOUNDING_BOX.getSize().y)
            , grid(
                  std::vector<IndexListType>(GRID_RESOLUTION * GRID_RESOLUTION))
        {
        }

        SpatialIndex(SpatialIndex&&) = default;
        SpatialIndex(const SpatialIndex&) = delete;
        ~SpatialIndex() = default;

    public:
        /**
         * \brief Remove an item stored at given index from
         * the spatial lookup so it is not returned by getOverlapCandidates
         *
         * \param id Index of the object within the buffer
         * \param box Collision box of the object
         *
         * It is recommended to call this function rather than eraseAtIndex
         * if you just want to move the item in 2D space.
         */
        template<AaBbType AABB>
        void removeFromLookup(IndexType id, const AABB& box)
        {
            foreachMatchingCellDo(
                box,
                [id](IndexListType& list) constexpr
                {
                    for (unsigned i = 0; i < list.size(); i++)
                    {
                        if (list[i] == id)
                        {
                            list[i] = list[list.size() - 1];
                            list.pop_back();
                            break;
                        }
                    }
                });
        }

        /**
         * \brief Return previously removed item to lookup
         *
         * \warn Only call this function after previous call
         * to removeFromLookup with the same id!
         */
        template<AaBbType AABB>
        inline void returnToLookup(IndexType id, const AABB& box)
        {
            foreachMatchingCellDo<AABB, false>(
                box,
                [id](IndexListType& list) constexpr
                {
                    // insert cell id into cell list
                    list.push_back(id);
                });
        }

        /**
         * \brief Get collection of ids of items that might be colliding with
         * given bounding box.
         *
         * If you don't want id of currently tested item to pop up in this
         * list, remove it first using removeFromLookup method.
         *
         * It is safe to call operator[] on every id returned by this method
         * until next call to erase that might delete given id.
         */
        template<AaBbType AABB>
        NODISCARD_RESULT std::vector<IndexType>
        getOverlapCandidates(const AABB& box) const
        {
            if (!dgm::Collision::basic(BOUNDING_BOX, box)) return {};

            auto&& result = std::vector<IndexType> {};
            result.reserve(32);

            foreachMatchingCellDo(
                box,
                [&result](const IndexListType& list) constexpr
                { result.insert(result.end(), list.begin(), list.end()); });

            if (result.empty()) return result;

            // Using sort+unique is faster than set or unordered_set
            std::sort(result.begin(), result.end());
            result.erase(
                std::unique(result.begin(), result.end()), result.end());
            return result;
        }

        NODISCARD_RESULT const constexpr dgm::Rect&
        getBoundingBox() const noexcept
        {
            return BOUNDING_BOX;
        }

        void clear()
        {
            for (auto&& cell : grid)
                cell.clear();
        }

    private:
        struct GridRect
        {
            unsigned x1, y1, x2, y2;
        };

        NODISCARD_RESULT constexpr sf::Vector2u
        getGridIndexFromCoord(const sf::Vector2f& coord) const noexcept
        {
            return {
                static_cast<unsigned>(std::clamp(
                    (coord.x - BOUNDING_BOX.getPosition().x) * COORD_TO_GRID_X,
                    0.f,
                    static_cast<float>(GRID_RESOLUTION - 1))),
                static_cast<unsigned>(std::clamp(
                    (coord.y - BOUNDING_BOX.getPosition().y) * COORD_TO_GRID_Y,
                    0.f,
                    static_cast<float>(GRID_RESOLUTION - 1)))
            };
        }

        NODISCARD_RESULT GridRect
        convertBoxToGridRect(const sf::Vector2f& point) const noexcept
        {
            const auto&& coord = getGridIndexFromCoord(point);
            return { coord.x, coord.y, coord.x, coord.y };
        }

        NODISCARD_RESULT GridRect
        convertBoxToGridRect(const dgm::Circle& box) const noexcept
        {
            auto&& center = box.getPosition();
            const auto&& radius =
                sf::Vector2f { box.getRadius(), box.getRadius() };
            const auto&& topLft = getGridIndexFromCoord(center - radius);
            const auto&& btmRgt = getGridIndexFromCoord(center + radius);

            return { topLft.x, topLft.y, btmRgt.x, btmRgt.y };
        }

        NODISCARD_RESULT GridRect
        convertBoxToGridRect(const dgm::Rect& box) const noexcept
        {
            const auto&& topLft = getGridIndexFromCoord(box.getPosition());
            const auto&& btmRgt =
                getGridIndexFromCoord(box.getPosition() + box.getSize());

            return { topLft.x, topLft.y, btmRgt.x, btmRgt.y };
        }

        template<class AABB, bool skipEmpty = true>
        constexpr void foreachMatchingCellDo(
            const AABB& box, std::function<void(IndexListType&)> callback)
        {
            const auto&& gridRect = convertBoxToGridRect(box);

            for (GridResolutionType y = gridRect.y1; y <= gridRect.y2; y++)
            {
                for (GridResolutionType x = gridRect.x1,
                                        index = y * GRID_RESOLUTION + x;
                     x <= gridRect.x2;
                     ++x, ++index)
                {
                    if constexpr (skipEmpty)
                    {
                        if (!grid[index].empty()) callback(grid[index]);
                    }
                    else
                    {
                        callback(grid[index]);
                    }
                }
            }
        }

        template<class AABB, bool skipEmpty = true>
        constexpr void foreachMatchingCellDo(
            const AABB& box,
            std::function<void(const IndexListType&)> callback) const
        {
            const auto&& gridRect = convertBoxToGridRect(box);

            for (GridResolutionType y = gridRect.y1; y <= gridRect.y2; y++)
            {
                for (GridResolutionType x = gridRect.x1,
                                        index = y * GRID_RESOLUTION + x;
                     x <= gridRect.x2;
                     ++x, ++index)
                {
                    if constexpr (skipEmpty)
                    {
                        if (!grid[index].empty()) callback(grid[index]);
                    }
                    else
                    {
                        callback(grid[index]);
                    }
                }
            }
        }

    private:
        const dgm::Rect BOUNDING_BOX;
        const GridResolutionType GRID_RESOLUTION;
        const float COORD_TO_GRID_X;
        const float COORD_TO_GRID_Y;
        std::vector<IndexListType> grid;
    };

} // namespace dgm