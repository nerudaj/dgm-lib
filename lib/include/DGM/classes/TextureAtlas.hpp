#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Compatibility.hpp>
#include <DGM/classes/Error.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>

namespace dgm
{
    class [[nodiscard]] TextureAtlas final
    {
    public:
        template<class T>
        struct [[nodiscard]] ResourceLocation final
        {
        private:
            constexpr ResourceLocation(size_t idx) : idx(idx) {}

        private:
            size_t idx;

            friend class TextureAtlas;
        };

    public:
        TextureAtlas(int atlasWidth, int atlasHeight);

    public:
        /// <summary>
        /// Add a texture to atlas as a single image
        /// </summary>
        ///
        /// Clip for this texture will have single frame encompassing
        /// the whole texture.
        NODISCARD_RESULT std::expected<ResourceLocation<dgm::Clip>, dgm::Error>
        addImage(const sf::Texture& texture);

        NODISCARD_RESULT std::expected<ResourceLocation<dgm::Clip>, dgm::Error>
        addTileset(const sf::Texture& texture, const dgm::Clip& clip);

        NODISCARD_RESULT
        std::expected<ResourceLocation<dgm::AnimationStates>, dgm::Error>
        addSpritesheet(
            const sf::Texture& texture, const dgm::AnimationStates& states);

        CONSTEXPR_NODISCARD const sf::Texture& getTexture() const noexcept
        {
            return atlasTexture;
        }

        NODISCARD_RESULT const dgm::Clip&
        getClip(const ResourceLocation<dgm::Clip>& location) const noexcept
        {
            return clips[location.idx];
        }

        NODISCARD_RESULT const dgm::AnimationStates& getAnimationStates(
            const ResourceLocation<dgm::AnimationStates>& location)
            const noexcept
        {
            return states[location.idx];
        }

    private:
        static std::vector<sf::IntRect> subdivideArea(
            const sf::IntRect& area, const sf::Vector2i& takenTextureDims);

        static constexpr bool
        fits(const sf::Vector2i& textureDims, const sf::IntRect& area) noexcept
        {
            // NOTE: don't use sf::IntRect::contains
            // - too much overhead
            return textureDims.x <= area.size.x && textureDims.y <= area.size.y;
        }

        void adjustFreeArea(
            std::vector<sf::IntRect>::const_iterator&& freeAreaItr,
            const sf::Vector2i& takenTextureDims);

        static dgm::Clip recomputeClip(
            const dgm::Clip& clip,
            const sf::Vector2i& startCoord,
            const sf::Vector2i& textureSize);

        static dgm::AnimationStates recomputeAnimationStates(
            const dgm::AnimationStates& animationStates,
            const sf::Vector2i& startCoord,
            const sf::Vector2i& textureSize);

        void copyTexture(
            const sf::Texture& textureToCopy, const sf::Vector2i& offset);

    private:
        sf::Texture atlasTexture;
        sf::Image atlasImage;
        std::vector<dgm::Clip> clips;
        std::vector<dgm::AnimationStates> states;
        std::vector<sf::IntRect> freeAreas;
    };
} // namespace dgm