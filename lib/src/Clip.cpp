#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Utility.hpp>
#include <cassert>
#include <iostream>

void dgm::Clip::init(
    const sf::Vector2u& frameSize,
    const sf::IntRect& boundaries,
    const std::size_t frameCount,
    const sf::Vector2u& frameSpacing)
{
    assert(frameSize.x > 0 and frameSize.y > 0);
    assert(boundaries.position.x >= 0 and boundaries.position.y >= 0);
    assert(boundaries.size.x > 0 and boundaries.size.y > 0);
    assert(frameSize.x <= static_cast<unsigned>(boundaries.size.x));
    assert(frameSize.y <= static_cast<unsigned>(boundaries.size.y));

    // There will always be at least 1 frame, otherwise asserts will fail
    //
    // To compute the rest we need to solve following equatations:
    // BW >= frameSize.x * K + frameSpacing.x * L
    // K = L + 1
    // BW >= L * frameSize.x + frameSize.x + L * frameSpacing.x
    // BW - frameSize.x >= L * ( frameSize.x + frameSpacing.x )
    const std::size_t X_FRAME_COUNT =
        1
        + (static_cast<std::size_t>(boundaries.size.x)
           - static_cast<std::size_t>(frameSize.x))
              / (static_cast<std::size_t>(frameSize.x)
                 + static_cast<std::size_t>(frameSpacing.x));
    const std::size_t Y_FRAME_COUNT =
        1
        + (static_cast<std::size_t>(boundaries.size.y)
           - static_cast<std::size_t>(frameSize.y))
              / (static_cast<std::size_t>(frameSize.y)
                 + static_cast<std::size_t>(frameSpacing.y));
    const std::size_t MAX_AVAILABLE_FRAMES = X_FRAME_COUNT * Y_FRAME_COUNT;

    if (frameCount > MAX_AVAILABLE_FRAMES)
    {
        std::cerr << "dgm::Clip::init called with frame count '"
                         + std::to_string(frameCount) + "', but only '"
                         + std::to_string(MAX_AVAILABLE_FRAMES)
                         + "' frames are available given frameSize "
                         + dgm::Utility::to_string(frameSize) + ", boundaries ["
                         + dgm::Utility::to_string(boundaries.position) + ", "
                         + dgm::Utility::to_string(boundaries.size)
                         + "] and  frameSpacing "
                         + dgm::Utility::to_string(frameSpacing) + "\n";
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

    sf::Vector2i frameStart = boundaries.position;
    for (std::size_t y = 0, i = 0; y < Y_FRAME_COUNT; y++)
    {
        frameStart.x = boundaries.position.x;
        for (std::size_t x = 0; x < X_FRAME_COUNT && i < CONCRETE_FRAME_COUNT;
             x++, i++)
        {
            frames.push_back(sf::IntRect(frameStart, sf::Vector2i(frameSize)));

            frameStart.x += X_INCREMENT;
        }

        frameStart.y += Y_INCREMENT;
    }

    originalSpacing = frameSpacing;
}
