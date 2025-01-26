#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace dgm
{
    /**
     *  \brief Class representing array of sf::IntRect (frames) for purpose of
     * clipping textures
     *
     *  This class is base building block for animations or tilesets.
     *  All frames must have the same size
     */
    class Clip
    {
    public:
        /**
         *  \brief Get reference to selected frame
         *
         *  \param [in] index Index of the frame
         *
         *  This function throws if index is out of bounds.
         */
        [[nodiscard]] const sf::IntRect& getFrame(const std::size_t index) const
        {
            return frames.at(index);
        }

        /**
         * \brief Get reference to selected frame without checking array bounds
         *
         * \param [in] index Index of the frame
         */
        [[nodiscard]] const sf::IntRect&
        getFrameUnchecked(const std::size_t index) const noexcept
        {
            return frames[index];
        }

        /**
         *  \brief Get number of frames stored in object
         */
        [[nodiscard]] std::size_t getFrameCount() const noexcept
        {
            return frames.size();
        }

        /**
         *  \brief Get resolution of every frame in object
         */
        [[nodiscard]] const sf::Vector2u& getFrameSize() const noexcept
        {
            return size;
        }

        // TODO: only part of this can be computed dynamically
        [[nodiscard]] constexpr const sf::Vector2u&
        getOriginalSpacing() const noexcept
        {
            return originalSpacing;
        }

        /**
         *  \brief Initialize the object
         *
         *  \param [in] frameSize Size of the frame
         *  \param [in] boundaries Bounding box around all frames to load
         *  \param [in] frameCount Number of frames to load from boundaries
         *  \param [in] frameSpacing Offset between two neighbouring frames
         *
         *  \details This method must be called prior to any other method.
         *  If frameCount is 0, maximum number of frames is loaded within
         * bounding box
         */
        void init(
            const sf::Vector2u& frameSize,
            const sf::IntRect& boundaries,
            const std::size_t frameCount = 0,
            const sf::Vector2u& frameSpacing = sf::Vector2u(0, 0));

        [[nodiscard, deprecated]] Clip() = default;

        [[nodiscard]] Clip(
            const sf::Vector2u& frameSize,
            const sf::IntRect& boundaries,
            const std::size_t frameCount = 0,
            const sf::Vector2u& frameSpacing = sf::Vector2u(0, 0))
        {
            init(frameSize, boundaries, frameCount, frameSpacing);
        }

    protected:
        std::vector<sf::IntRect> frames; /// Array of sprite frames
        sf::Vector2u size;               /// Size of each frame in pixels
        sf::Vector2u originalSpacing;
    };
} // namespace dgm