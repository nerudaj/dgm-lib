#include <DGM/classes/Clip.hpp>
#include <cassert>
#include <format>
#include <iostream>

void dgm::Clip::init(
    const sf::Vector2u& frameSize,
    const sf::IntRect& boundaries,
    const std::size_t frameCount,
    const sf::Vector2u& frameSpacing)
{
    assert(frameSize.x > 0 and frameSize.y > 0);
    assert(boundaries.left >= 0 and boundaries.top >= 0);
    assert(boundaries.width > 0 and boundaries.height > 0);
    assert(frameSize.x <= static_cast<unsigned>(boundaries.width));
    assert(frameSize.y <= static_cast<unsigned>(boundaries.height));

    // There will always be at least 1 frame, otherwise asserts will fail
    //
    // To compute the rest we need to solve following equatations:
    // BW >= frameSize.x * K + frameSpacing.x * L
    // K = L + 1
    // BW >= L * frameSize.x + frameSize.x + L * frameSpacing.x
    // BW - frameSize.x >= L * ( frameSize.x + frameSpacing.x )
    const std::size_t X_FRAME_COUNT =
        1
        + (static_cast<std::size_t>(boundaries.width)
           - static_cast<std::size_t>(frameSize.x))
              / (static_cast<std::size_t>(frameSize.x)
                 + static_cast<std::size_t>(frameSpacing.x));
    const std::size_t Y_FRAME_COUNT =
        1
        + (static_cast<std::size_t>(boundaries.height)
           - static_cast<std::size_t>(frameSize.y))
              / (static_cast<std::size_t>(frameSize.y)
                 + static_cast<std::size_t>(frameSpacing.y));
    const std::size_t MAX_AVAILABLE_FRAMES = X_FRAME_COUNT * Y_FRAME_COUNT;

    if (frameCount > MAX_AVAILABLE_FRAMES)
    {
        std::cerr << std::format(
            "dgm::Clip::init called with frame count {}, but only {} frames "
            "are available"
            " given frameSize [{}, {}], boundaries [{}, {}, {}, {}] and "
            "frameSpacing [{}, {}]\n",
            frameCount,
            MAX_AVAILABLE_FRAMES,
            frameSize.x,
            frameSize.y,
            boundaries.left,
            boundaries.top,
            boundaries.width,
            boundaries.height,
            frameSpacing.x,
            frameSpacing.y);
    }

    const std::size_t CONCRETE_FRAME_COUNT =
        frameCount > 0
            ? (frameCount > MAX_AVAILABLE_FRAMES ? MAX_AVAILABLE_FRAMES
                                                 : frameCount)
            : MAX_AVAILABLE_FRAMES;

    frames.clear();
    frames.reserve(CONCRETE_FRAME_COUNT);
    size = frameSize;

    const int X_INCREMENT = frameSize.x + frameSpacing.x;
    const int Y_INCREMENT = frameSize.y + frameSpacing.y;

    int frameStartY = boundaries.top;
    for (std::size_t y = 0, i = 0; y < Y_FRAME_COUNT; y++)
    {
        int frameStartX = boundaries.left;
        for (std::size_t x = 0; x < X_FRAME_COUNT && i < CONCRETE_FRAME_COUNT;
             x++, i++)
        {
            frames.push_back(sf::IntRect(
                frameStartX, frameStartY, frameSize.x, frameSize.y));

            frameStartX += X_INCREMENT;
        }

        frameStartY += Y_INCREMENT;
    }

    originalSpacing = frameSpacing;
    originalOffset = sf::Vector2u {
        sf::Vector2i {
            boundaries.left,
            boundaries.top,
        },
    };
}
