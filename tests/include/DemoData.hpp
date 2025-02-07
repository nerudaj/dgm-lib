#pragma once

#include <DGM/classes/Animation.hpp>

static inline dgm::Clip createDummyClip()
{
    return dgm::Clip(
        { 32, 32 },
        sf::IntRect {
            sf::Vector2i(10, 20),
            sf::Vector2i(84, 40),
        },
        2,
        { 12, 12 });
}

static inline dgm::AnimationStates createDummySpritesheet()
{
    return { {
                 "first",
                 createDummyClip(),
             },
             {
                 "second",
                 dgm::Clip(
                     { 32, 32 },
                     sf::IntRect {
                         sf::Vector2i(6, 128),
                         sf::Vector2i(96, 32),
                     }),
             } };
}