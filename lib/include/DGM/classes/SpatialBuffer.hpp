#pragma once

#include <DGM/classes/DynamicBuffer.hpp>
#include <DGM/classes/Objects.hpp>
#include <DGM/classes/Traits.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

namespace dgm
{

    // mid and coarse will be automatically computed
    template<TrivialType T, unsigned FineResolution>
    class SpatialBuffer final
    {
    public:
        using AABB = dgm::Circle;
        using Index = std::size_t;
        using IndexList = std::list<Index>;

    public:
        SpatialBuffer(sf::FloatRect boundingBox)
            : boundingBox(std::move(boundingBox))
        {
            static_assert(FineResolution > 0);
        }

    public:
        void removeFromLookup(std::size_t id, const AABB& box)
        {
            foreachMatchingCellDo(
                box,
                [id](IndexList& list) constexpr
                {
                    // erase id from list
                    list.remove(id);
                });
        }

        inline void addToLookup(std::size_t id, const AABB& box)
        {
            foreachMatchingCellDo(
                box,
                [id](IndexList& list) constexpr
                {
                    // insert cell id into cell list
                    list.push_back(id);
                });
        }

        std::size_t insert(T item, const AABB& box)
        {
            auto&& index = items.pushBack(item);
            addToLookup(index, box);
            return index;
        }

        void erase(std::size_t id, const AABB& box)
        {
            items.erase(id);
            removeFromLookup(id, box);
        }

        std::set<std::size_t> getOverlapCandidates(const AABB& box)
        {
            auto&& result = std::set<std::size_t> {};

            foreachMatchingCellDo(
                box,
                [&result](IndexList& list) constexpr
                {
                    // set will make sure ids are deduped
                    result.insert(list.begin(), list.end());
                });

            return result;
        }

        template<class Self>
        auto&& operator[](this Self&& self, std::size_t id)
        {
            return self.items[id];
        }

    private:
        struct GridRect
        {
            unsigned x1, y1, x2, y2;
        };

        [[nodiscard]] constexpr sf::Vector2u
        getGridIndexFromCoord(const sf::Vector2f& coord) noexcept
        {
            return { static_cast<unsigned>(std::clamp(
                         (coord.x - boundingBox.left) * FineResolution
                             / boundingBox.width,
                         0.f,
                         static_cast<float>(FineResolution))),
                     static_cast<unsigned>(std::clamp(
                         (coord.y - boundingBox.top) * FineResolution
                             / boundingBox.height,
                         0.f,
                         static_cast<float>(FineResolution))) };
        }

        GridRect convertBoxToGridRect(const AABB& box)
        {
            auto&& center = box.getPosition();
            auto&& radius = sf::Vector2f { box.getRadius(), box.getRadius() };
            auto&& topLft = getGridIndexFromCoord(center - radius);
            auto&& btmRgt = getGridIndexFromCoord(center + radius);

            return { topLft.x, topLft.y, btmRgt.x, btmRgt.y };
        }

        constexpr void foreachMatchingCellDo(
            const AABB& box, std::function<constexpr void(IndexList&)> callback)
        {
            auto&& gridRect = convertBoxToGridRect(box);

            for (auto&& y = gridRect.y1; y <= gridRect.y2; y++)
            {
                for (auto&& x = gridRect.x1; x <= gridRect.x2; x++)
                {
                    auto&& index = y * FineResolution + x;
                    callback(grid[index]);
                }
            }
        }

    private:
        const sf::FloatRect boundingBox;
        DynamicBuffer<T> items;
        std::vector<IndexList> grid =
            std::vector<IndexList>(FineResolution * FineResolution);
    };

} // namespace dgm

/*
SpatialBuffer<Actor, 1000> actors(areaBoundingBox);

for (unsigned i = 0; i < 100; i++)
    actors.insert(getActor(i));

for (auto&& [actor, id] : actors)
{
    actors.removeFromLookup(id);

    // transformation done to the actor
    if (actors.hasOverlap(actor.body))
        // do something else

    if (actor.isDead())
        actors.erase(id);
    else
        actors.addToLookup(id);
}

// when item is erased, it must be taken from lookup
*/