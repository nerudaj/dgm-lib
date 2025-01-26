#include <DGM/classes/TextureAtlas.hpp>

using ClipLocation = dgm::TextureAtlas::ResourceLocation<dgm::Clip>;

using SheetLocation = dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>;

dgm::TextureAtlas::TextureAtlas(int atlasWidth, int atlasHeight)
{
    if (!texture.create(atlasWidth, atlasHeight))
        throw dgm::Exception("Could not create underlying atlas texture");

    freeAreas.push_back({ 0, 0, atlasWidth, atlasHeight });
}

std::expected<ClipLocation, dgm::Error>
dgm::TextureAtlas::addTileset(const sf::Texture& tileset, const dgm::Clip& clip)
{
    const auto&& size = tileset.getSize();

    auto&& itr = std::find_if(
        freeAreas.begin(),
        freeAreas.end(),
        [&](auto&& area) { return fits(size, area); });
    if (itr == freeAreas.end())
        return std::unexpected(dgm::Error("No free space left in the atlas!"));

    const auto&& startCoord = sf::Vector2u {
        sf::Vector2i {
            itr->left,
            itr->top,
        },
    };

    adjustFreeArea(std::move(itr), sf::Vector2i(size));
    clips.push_back(recomputeClip(clip, startCoord, size));

    return ClipLocation(clips.size() - 1);
}

std::expected<SheetLocation, dgm::Error> dgm::TextureAtlas::addSpritesheet(
    const sf::Texture& spritesheet, const dgm::AnimationStates& ss)
{
    // TODO: this
    return SheetLocation(0);
}

void dgm::TextureAtlas::adjustFreeArea(
    std::vector<sf::IntRect>::const_iterator&& freeAreaItr,
    const sf::Vector2i& takenTextureDims)
{
    auto&& subdividedFreeAreas = subdivideArea(*freeAreaItr, takenTextureDims);
    freeAreas.erase(freeAreaItr);
    freeAreas.insert(
        subdividedFreeAreas.begin(),
        subdividedFreeAreas.end(),
        freeAreas.end());
}

std::vector<sf::IntRect> dgm::TextureAtlas::subdivideArea(
    const sf::IntRect& area, const sf::Vector2i& takenTextureDims)
{
    auto&& result = std::vector<sf::IntRect>();

    if (takenTextureDims.x < area.width)
    {
        result.push_back(sf::IntRect {
            takenTextureDims.x,
            0,
            area.width - takenTextureDims.x,
            takenTextureDims.y,
        });
    }

    if (takenTextureDims.y < area.height)
    {
        result.push_back(sf::IntRect {
            0,
            takenTextureDims.y,
            area.width,
            area.height - takenTextureDims.y,
        });
    }

    return result;
}

dgm::Clip dgm::TextureAtlas::recomputeClip(
    const dgm::Clip& clip,
    const sf::Vector2u& startCoord,
    const sf::Vector2u& textureSize)
{
    const auto& offset = clip.getOriginalOffset();

    return dgm::Clip(
        clip.getFrameSize(),
        sf::IntRect {
            static_cast<int>(startCoord.x + offset.x),
            static_cast<int>(startCoord.y + offset.y),
            static_cast<int>(textureSize.x),
            static_cast<int>(textureSize.y),
        },
        clip.getFrameCount(),
        clip.getOriginalSpacing());
}
