#pragma once
#include <SFML/System/Vector2.hpp>

namespace dgm
{

    namespace priv
    {

        sf::Vector2u advanceUp(const sf::Vector2u& point)
        {
            return { point.x, point.y - 1 };
        };

        sf::Vector2u advanceUpRight(const sf::Vector2u& point)
        {
            return { point.x + 1, point.y - 1 };
        };

        sf::Vector2u advanceRight(const sf::Vector2u& point)
        {
            return { point.x + 1, point.y };
        };

        sf::Vector2u advanceDownRight(const sf::Vector2u& point)
        {
            return { point.x + 1, point.y + 1 };
        };

        sf::Vector2u advanceDown(const sf::Vector2u& point)
        {
            return { point.x, point.y + 1 };
        };

        sf::Vector2u advanceDownLeft(const sf::Vector2u& point)
        {
            return { point.x - 1, point.y + 1 };
        };

        sf::Vector2u advanceLeft(const sf::Vector2u& point)
        {
            return { point.x - 1, point.y };
        };

        sf::Vector2u advanceUpLeft(const sf::Vector2u& point)
        {
            return { point.x - 1, point.y - 1 };
        };

        bool shouldStopStraightDiscovery(
            const sf::Vector2u& point, const dgm::Mesh& mesh)
        {
            return mesh[point] > 0;
        }

        bool shouldStopUpRightDiscovery(
            const sf::Vector2u& point, const dgm::Mesh& mesh)
        {
            return mesh[point] > 0 || mesh[{ point.x - 1, point.y }] > 0
                   || mesh[{ point.x, point.y + 1 }] > 0;
        }

        bool shouldStopDownRightDiscovery(
            const sf::Vector2u& point, const dgm::Mesh& mesh)
        {
            return mesh[point] > 0 || mesh[{ point.x - 1, point.y }] > 0
                   || mesh[{ point.x, point.y - 1 }] > 0;
        }

        bool shouldStopDownLeftDiscovery(
            const sf::Vector2u& point, const dgm::Mesh& mesh)
        {
            return mesh[point] > 0 || mesh[{ point.x + 1, point.y }] > 0
                   || mesh[{ point.x, point.y - 1 }] > 0;
        }

        bool shouldStopUpLeftDiscovery(
            const sf::Vector2u& point, const dgm::Mesh& mesh)
        {
            return mesh[point] > 0 || mesh[{ point.x + 1, point.y }] > 0
                   || mesh[{ point.x, point.y + 1 }] > 0;
        }

    } // namespace priv

} // namespace dgm