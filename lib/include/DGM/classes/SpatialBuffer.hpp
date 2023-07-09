#pragma once

#include <DGM/classes/Collision.hpp>
#include <DGM/classes/DynamicBuffer.hpp>
#include <DGM/classes/Objects.hpp>
#include <SFML/Graphics/Rect.hpp>
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

    // clang-format off
    /**
     * \brief Data storage container that provides efficient 2D spatial lookup
     * for items
     *
     * \tparam T Type of the objects stored in the buffer. They have to be
     * default-constructible and swappable
     *
     * \tparam FineResolution Resolution of
     * inner indexing grid. Should be the half of the bounding box
     *
     * \tparam IndexType This structure uses a lot of numerical indexes. You can
     * specify narrower type than std::size_t to save on some memory and
     * potentially improve cache locations
     *
     * Similar to quad tree, you can use this structure to store items
     * and look them up based on given collision box. This buffer will provide
     * you with a list of items that might collide with provided collision box.
     *
     * Use insert/eraseAtIndex methods to add and remove items, similar to any
     * other collection. When you want to move an item spatially, first call
     * removeFromLookup, move the item and then call returnToLookup, you don't
     * have to eraseAtIndex/reinsert completely.
     *
     * Internally, a dgm::DynamicBuffer is used to store the items themselves
     * and a dense grid is used to handle the spatial indexing. Indices are
     * stable - if you delete an item, all other indices remain unaffected.
     * However, calling operator[] with index of a deleted item will crash the
     * program.
     *
     * Recommended way of using this structure:
     *
     * \code
     * for (auto&& [item, id] : buffer)
     * {
     *     buffer.removeFromLookup(id, item.collisionBox);
     *     for (auto&& candidateId : buffer.getOverlapCandidates(item.collisionBox))
     *     {
     *          auto&& candidate = buffer[candidateId];
     *          // compute collisions, move item
     *     }
     *     buffer.returnToLookup(id, item.collisionBox);
     * }
     * \endcode
     */
    // clang-format on
    template<
        class T,
        typename IndexType = std::size_t,
        typename GridResolutionType = unsigned>
    class SpatialBuffer final
    {
    public:
        using IndexListType = std::vector<IndexType>;
        using StorageType = dgm::DynamicBuffer<T, 1024, IndexType>;

    public:
        [[nodiscard]] constexpr SpatialBuffer(
            dgm::Rect boundingBox, GridResolutionType gridResolution)
            : BOUNDING_BOX(std::move(boundingBox))
            , GRID_RESOLUTION(gridResolution)
            , COORD_TO_GRID_X(gridResolution / BOUNDING_BOX.getSize().x)
            , COORD_TO_GRID_Y(gridResolution / BOUNDING_BOX.getSize().y)
        {
            grid =
                std::vector<IndexListType>(GRID_RESOLUTION * GRID_RESOLUTION);
        }

        [[nodiscard]] SpatialBuffer(SpatialBuffer&&) = default;
        SpatialBuffer(const SpatialBuffer&) = delete;
        ~SpatialBuffer() = default;

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
         * \brief Add a new item to collection
         *
         * \param item Item to insert
         * \param box Collision box of the item
         *
         * \return Index of the inserted item
         * that can be used with calls to
         * removeFromLookup, returnToLookup, eraseAtIndex or
         * operator[]. This index can also be retrieved from
         * iterators.
         */
        template<AaBbType AABB>
        IndexType insert(T item, const AABB& box)
        {
            auto&& index = items.emplaceBack(item);
            returnToLookup(index, box);
            return index;
        }

        /**
         * \brief Delete an item with given id and collision box from
         * the memory
         *
         * \param id Index of the item
         * \param box Collision box of the item
         *
         * Only use this method when you want to get rid of an item
         * completely. If you just want to move it, use removeFromLookup
         * insted.
         */
        template<AaBbType AABB>
        void eraseAtIndex(IndexType id, const AABB& box)
        {
            items.eraseAtIndex(id);
            removeFromLookup(id, box);
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
        std::vector<IndexType> getOverlapCandidates(const AABB& box)
        {
            if (!dgm::Collision::basic(BOUNDING_BOX, box)) return {};

            auto&& result = std::vector<IndexType> {};
            result.reserve(32);

            foreachMatchingCellDo(
                box,
                [&result](IndexListType& list) constexpr
                { result.insert(result.end(), list.begin(), list.end()); });

            if (result.empty()) return result;

            // Using sort+unique is faster than set or unordered_set
            std::sort(result.begin(), result.end());
            result.erase(
                std::unique(result.begin(), result.end()), result.end());
            return result;
        }

        template<class Self>
        auto&& operator[](this Self&& self, IndexType id)
        {
            return self.items[id];
        }

        [[nodiscard]] constexpr StorageType::iterator begin() noexcept
        {
            return items.begin();
        }

        [[nodiscard]] constexpr StorageType::iterator end() noexcept
        {
            return items.end();
        }

    private:
        struct GridRect
        {
            unsigned x1, y1, x2, y2;
        };

        [[nodiscard]] constexpr sf::Vector2u
        getGridIndexFromCoord(const sf::Vector2f& coord) noexcept
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

        [[nodiscard]] GridRect
        convertBoxToGridRect(const sf::Vector2f& point) noexcept
        {
            const auto&& coord = getGridIndexFromCoord(point);
            return { coord.x, coord.y, coord.x, coord.y };
        }

        [[nodiscard]] GridRect
        convertBoxToGridRect(const dgm::Circle& box) noexcept
        {
            auto&& center = box.getPosition();
            const auto&& radius =
                sf::Vector2f { box.getRadius(), box.getRadius() };
            const auto&& topLft = getGridIndexFromCoord(center - radius);
            const auto&& btmRgt = getGridIndexFromCoord(center + radius);

            return { topLft.x, topLft.y, btmRgt.x, btmRgt.y };
        }

        [[nodiscard]] GridRect
        convertBoxToGridRect(const dgm::Rect& box) noexcept
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

    private:
        const dgm::Rect BOUNDING_BOX;
        const GridResolutionType GRID_RESOLUTION;
        const float COORD_TO_GRID_X;
        const float COORD_TO_GRID_Y;
        StorageType items;
        std::vector<IndexListType> grid;
    };

} // namespace dgm
