#pragma once

#include <DGM\dgm.hpp>
#include <vector>

namespace dgm {
	/**
	 *  \brief Class representing array of sf::IntRect (frames) for purpose of clipping textures
	 *  
	 *  This class is base building block for animations or tilesets.
	 */
	class Clip {
	protected:
		std::vector<sf::IntRect> frames;
		sf::Vector2u size;

	public:
		/**
		 *  \brief Get reference to selected frame frame
		 *
		 *  \param [in] index Index of the frame
		 */
		const sf::IntRect &getFrame(const std::size_t index) const { return frames[index]; }
		
		/**
		 *  \brief Get number of frames stored in object
		 */
		std::size_t getFrameCount() const { return frames.size(); }
		
		/**
		 *  \brief Get resolution of every frame in object
		 */
		const sf::Vector2u &getFrameSize() const { return size; }

		/**
		 *  \brief Initialize the object
		 *
		 *  \param [in] frameSize Size of the frame
		 *  \param [in] boundaries Bounding box around all frames to load
		 *  \param [in] frameCount Number of frames to load from boundaries
		 *  \param [in] frameOffset Offset between two neighbouring frames
		 *
		 *  \details This method must be called prior to any other method.
		 *  If frameCount is 0, maximum number of frames is loaded within bounding box
		 */
		bool init(const sf::Vector2u &frameSize, const sf::IntRect &boundaries, std::size_t frameCount = 0, const sf::Vector2u &frameOffset = sf::Vector2u(0, 0));

		Clip();
		Clip(const sf::Vector2u &frameSize, const sf::IntRect &boundaries, std::size_t frameCount = 0, const sf::Vector2u &frameOffset = sf::Vector2u(0, 0));
		~Clip();
	};
}