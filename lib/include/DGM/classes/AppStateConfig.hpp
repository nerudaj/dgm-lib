#pragma once

#include <SFML/Graphics/Color.hpp>

namespace dgm
{
    struct [[nodiscard]] AppStateConfig
    {
        const sf::Color clearColor =
            sf::Color::Black; ///< Return color that will be used as window
                              ///< clear color for the next frame.
        const bool shouldUpdateUnderlyingState =
            false; ///< Whether input/update methods will be called for state
                   ///< directly under current state before input()/update() of
                   ///< the current state
        const bool shouldDrawUnderlyingState =
            false; ///< Whether draw method will be called for state directly
                   ///< under current state before draw() of the current state
    };
} // namespace dgm