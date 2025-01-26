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
    const auto&& size = sf::Vector2i(tileset.getSize());

    auto&& itr = std::find_if(
        freeAreas.begin(),
        freeAreas.end(),
        [&](auto&& area) { return fits(size, area); });
    if (itr == freeAreas.end())
        return std::unexpected(dgm::Error("No free space left in the atlas!"));

    const auto&& startCoord = sf::Vector2i {
        itr->left,
        itr->top,
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
        freeAreas.end(),
        subdividedFreeAreas.begin(),
        subdividedFreeAreas.end());
}

std::vector<sf::IntRect> dgm::TextureAtlas::subdivideArea(
    const sf::IntRect& area, const sf::Vector2i& takenTextureDims)
{
    auto&& result = std::vector<sf::IntRect>();

    if (takenTextureDims.x < area.width)
    {
        result.push_back(sf::IntRect {
            area.left + takenTextureDims.x,
            area.top,
            area.width - takenTextureDims.x,
            takenTextureDims.y,
        });
    }

    if (takenTextureDims.y < area.height)
    {
        result.push_back(sf::IntRect {
            area.left,
            area.top + takenTextureDims.y,
            area.width,
            area.height - takenTextureDims.y,
        });
    }

    return result;
}

dgm::Clip dgm::TextureAtlas::recomputeClip(
    const dgm::Clip& clip,
    const sf::Vector2i& startCoord,
    const sf::Vector2i& textureSize)
{
    return dgm::Clip(
        clip.getFrameSize(),
        sf::IntRect {
            startCoord + clip.getFrame(0).getPosition(),
            textureSize,
        },
        clip.getFrameCount(),
        clip.getOriginalSpacing());
}
