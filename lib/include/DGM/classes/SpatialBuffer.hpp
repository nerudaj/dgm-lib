#pragma once

#include <DGM/classes/DynamicBuffer.hpp>
#include <DGM/classes/Objects.hpp>
#include <DGM/classes/SpatialIndex.hpp>

namespace dgm
{

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
    class [[nodiscard]] SpatialBuffer final
        : public SpatialIndex<IndexType, GridResolutionType>
    {
    public:
        using super = SpatialIndex<IndexType, GridResolutionType>;
        using DataType = T;
        using StorageType = dgm::DynamicBuffer<T, IndexType>;

    public:
        constexpr SpatialBuffer(
            dgm::Rect boundingBox, GridResolutionType gridResolution)
            : super(boundingBox, gridResolution), items(1024)
        {
        }

        SpatialBuffer(SpatialBuffer&&) = default;
        SpatialBuffer(const SpatialBuffer&) = delete;
        ~SpatialBuffer() = default;

    public:
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
        IndexType insert(T&& item, const AABB& box)
        {
            auto&& index = items.emplaceBack(std::forward<T>(item));
            super::returnToLookup(index, box);
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
            super::removeFromLookup(id, box);
        }

        [[nodiscard]] bool isIndexValid(IndexType idx) const
        {
            return items.isIndexValid(idx);
        }

#ifdef ANDROID
        T& operator[](IndexType id)
        {
            return items[id];
        }

        const T& operator[](IndexType id) const
        {
            return items[id];
        }
#else
        auto&& operator[](this auto&& self, IndexType id)
        {
            return self.items[id];
        }
#endif

        [[nodiscard]] constexpr StorageType::iterator begin() noexcept
        {
            return items.begin();
        }

        [[nodiscard]] constexpr StorageType::iterator end() noexcept
        {
            return items.end();
        }

    private:
        StorageType items;
    };

} // namespace dgm
