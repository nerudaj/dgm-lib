#pragma once

#include <DGM\dgm.hpp>
#include <cassert>

bool dgm::Clip::init(const sf::Vector2u & frameSize, const sf::IntRect & boundaries, std::size_t frameCount, const sf::Vector2u & frameOffset) {
	assert(frameSize.x > 0 and frameSize.y > 0);
	assert(boundaries.width > 0 and boundaries.height > 0);


	if (frameCount == 0) {
		sf::Vector2u blockSize = frameSize;
		sf::Vector2u chunkSize(boundaries.width, boundaries.height);

		if (blockSize.x + frameOffset.x <= chunkSize.x) blockSize.x += frameOffset.x;
		if (blockSize.y + frameOffset.y <= chunkSize.y) blockSize.y += frameOffset.y;

		frameCount = size_t(chunkSize.x / blockSize.x) * size_t(chunkSize.y / blockSize.y);
	}

	try {
		frames.resize(frameCount);
	}
	catch (std::bad_alloc &e) {
		std::cerr << "ERROR:Clip: " << e.what() << ".\n";
		return false;
	}

	Clip::size = frameSize;

	unsigned int i = 0;
	for (unsigned long y = boundaries.top; y < unsigned long(boundaries.top + boundaries.height); y += (frameSize.y + frameOffset.y)) {
		for (unsigned long x = boundaries.left; x < unsigned long(boundaries.left + boundaries.width); x += (frameSize.x + frameOffset.x)) {
			frames[i] = sf::IntRect(x, y, frameSize.x, frameSize.y);
			i++;

			if (i == frameCount) {
				x = (boundaries.left + boundaries.width);
				y = (boundaries.top + boundaries.height);
				break;
			}
		}
	}

	return true;
}

dgm::Clip::Clip() {
}

dgm::Clip::Clip(const sf::Vector2u & frameSize, const sf::IntRect & boundaries, std::size_t frameCount, const sf::Vector2u & frameOffset) {
	init(frameSize, boundaries, frameCount, frameOffset);
}

dgm::Clip::~Clip() {
}
