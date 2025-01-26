#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Error.hpp>
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
        std::expected<ResourceLocation<dgm::Clip>, dgm::Error>
        addTileset(const sf::Texture& tilesetp, const dgm::Clip& clip);

        std::expected<ResourceLocation<dgm::AnimationStates>, dgm::Error>
        addSpritesheet(
            const sf::Texture& spritesheet, const dgm::AnimationStates& states);

        const sf::Texture& getTexture() const noexcept
        {
            return texture;
        }

        const dgm::Clip&
        getClip(const ResourceLocation<dgm::Clip>& location) const noexcept
        {
            return clips[location.idx];
        }

        const dgm::AnimationStates& getAnimationStates(
            const ResourceLocation<dgm::AnimationStates>& location)
            const noexcept
        {
            return states[location.idx];
        }

        constexpr bool fits(
            const sf::Vector2u& textureDims,
            const sf::IntRect& area) const noexcept
        {
            return textureDims.x < static_cast<unsigned>(area.width)
                   && textureDims.y < static_cast<unsigned>(area.height);
        }

        void adjustFreeArea(
            std::vector<sf::IntRect>::const_iterator&& freeAreaItr,
            const sf::Vector2i& takenTextureDims);

        std::vector<sf::IntRect> subdivideArea(
            const sf::IntRect& area, const sf::Vector2i& takenTextureDims);

        dgm::Clip recomputeClip(
            const dgm::Clip& clip,
            const sf::Vector2u& startCoord,
            const sf::Vector2u& textureSize);

    private:
        sf::Texture texture;
        std::vector<dgm::Clip> clips;
        std::vector<dgm::AnimationStates> states;
        std::vector<sf::IntRect> freeAreas;
    };
} // namespace dgm