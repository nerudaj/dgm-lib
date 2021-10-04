#pragma once

#include <DGM\dgm.hpp>
#include <cassert>
//#include <format.h>

void dgm::Clip::init(const sf::Vector2u & frameSize, const sf::IntRect & boundaries, const std::size_t frameCount, const sf::Vector2u & frameOffset) {
	assert(frameSize.x > 0 and frameSize.y > 0);
	assert(boundaries.left >= 0 and boundaries.top >= 0);
	assert(boundaries.width > 0 and boundaries.height > 0);

	// solve following expressions:
	// K * frameSize.x + L * frameOffset.x <= boundaries.width - boundaries.top
	// K - L <= 1
	// K - L > 0
	// =>
	// x * frameSize.x + (x-1) * frameOffset.x <= B_W
	// x * (frameSize.x + frameOffset.x) <= B_W + frameOffset.x

	const std::size_t USABLE_WIDTH = static_cast<std::size_t>(boundaries.width) - static_cast<std::size_t>(boundaries.left);
	const std::size_t USABLE_HEIGHT = static_cast<std::size_t>(boundaries.height) - static_cast<std::size_t>(boundaries.top);
	const std::size_t X_FRAME_COUNT = USABLE_WIDTH / (static_cast<std::size_t>(frameSize.x) + static_cast<std::size_t>(frameOffset.x));
	const std::size_t Y_FRAME_COUNT = USABLE_HEIGHT / (static_cast<std::size_t>(frameSize.y) + static_cast<std::size_t>(frameOffset.y));
	const std::size_t MAX_AVAILABLE_FRAMES = X_FRAME_COUNT * Y_FRAME_COUNT;

	if (frameCount > MAX_AVAILABLE_FRAMES) {
		/*TODO:std::cerr << std::format("dgm::Clip::init called with frame count {}, but only {} frames are available"
								 " given frameSize [{}, {}], boundaries [{}, {}, {}, {}] and frameOffset [{}, {}]\n",
								 frameCount, MAX_AVAILABLE_FRAMES,
								 frameSize.x, frameSize.y,
								 boundaries.left, boundaries.top, boundaries.width, boundaries.height,
								 frameOffset.x, frameOffset.y);*/
	}

	const std::size_t CONCRETE_FRAME_COUNT = frameCount > 0
												? (frameCount > MAX_AVAILABLE_FRAMES ? MAX_AVAILABLE_FRAMES : frameCount)
												: MAX_AVAILABLE_FRAMES;

	frames.clear();
	frames.reserve(CONCRETE_FRAME_COUNT);
	size = frameSize;

	const int X_INCREMENT = frameSize.x + frameOffset.x;
	const int Y_INCREMENT = frameSize.y + frameOffset.y;

	int frameStartY = boundaries.top;
	for (std::size_t y = 0, i = 0; y < Y_FRAME_COUNT; y++) {
		int frameStartX = boundaries.left;
		for (std::size_t x = 0; x < X_FRAME_COUNT && i < CONCRETE_FRAME_COUNT; x++, i++) {
			frames.push_back(sf::IntRect(frameStartX, frameStartY, frameSize.x, frameSize.y));
			
			frameStartX += X_INCREMENT;
		}

		frameStartY += Y_INCREMENT;
	}
}
