#include <DGM/classes/TextureAtlas.hpp>
#include <ranges>

using ClipLocation = dgm::TextureAtlas::ResourceLocation<dgm::Clip>;

using SheetLocation = dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>;

dgm::TextureAtlas::TextureAtlas(int atlasWidth, int atlasHeight)
{
    atlasImage = sf::Image(sf::Vector2u {
        sf::Vector2i {
            atlasWidth,
            atlasHeight,
        },
    });

    freeAreas.push_back(sf::IntRect {
        sf::Vector2i(0, 0),
        sf::Vector2i(atlasWidth, atlasHeight),
    });
}

NODISCARD_RESULT std::expected<ClipLocation, dgm::Error>
dgm::TextureAtlas::addImage(const sf::Texture& texture)
{
    return addTileset(
        texture,
        dgm::Clip(
            texture.getSize(),
            sf::IntRect {
                sf::Vector2i { 0, 0 },
                sf::Vector2i(texture.getSize()),
            }));
}

std::expected<ClipLocation, dgm::Error>
dgm::TextureAtlas::addTileset(const sf::Texture& texture, const dgm::Clip& clip)
{
    const auto&& size = sf::Vector2i(texture.getSize());

    auto&& itr = std::find_if(
        freeAreas.begin(),
        freeAreas.end(),
        [&](auto&& area) { return fits(size, area); });
    if (itr == freeAreas.end())
        return std::unexpected(dgm::Error("No free space left in the atlas!"));

    const auto startCoord = itr->position;

    adjustFreeArea(std::move(itr), size);
    copyTexture(texture, startCoord);
    clips.push_back(recomputeClip(clip, startCoord, size));

    return ClipLocation(clips.size() - 1);
}

std::expected<SheetLocation, dgm::Error> dgm::TextureAtlas::addSpritesheet(
    const sf::Texture& texture, const dgm::AnimationStates& animStates)
{
    const auto&& size = sf::Vector2i(texture.getSize());

    auto&& itr = std::find_if(
        freeAreas.begin(),
        freeAreas.end(),
        [&](auto&& area) { return fits(size, area); });
    if (itr == freeAreas.end())
        return std::unexpected(dgm::Error("No free space left in the atlas!"));

    const auto startCoord = itr->position;

    adjustFreeArea(std::move(itr), size);
    copyTexture(texture, startCoord);
    states.push_back(recomputeAnimationStates(animStates, startCoord, size));

    return SheetLocation(states.size() - 1);
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

    if (takenTextureDims.x < area.size.x)
    {
        result.push_back(sf::IntRect {
            sf::Vector2i {
                area.position.x + takenTextureDims.x,
                area.position.y,
            },
            sf::Vector2i {
                area.size.x - takenTextureDims.x,
                takenTextureDims.y,
            },
        });
    }

    if (takenTextureDims.y < area.size.y)
    {
        result.push_back(sf::IntRect {
            sf::Vector2i {
                area.position.x,
                area.position.y + takenTextureDims.y,
            },
            sf::Vector2i {
                area.size.x,
                area.size.y - takenTextureDims.y,
            },
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
            startCoord + clip.getFrame(0).position,
            textureSize,
        },
        clip.getFrameCount(),
        clip.getOriginalSpacing());
}

dgm::AnimationStates dgm::TextureAtlas::recomputeAnimationStates(
    const dgm::AnimationStates& animationStates,
    const sf::Vector2i& startCoord,
    const sf::Vector2i& textureSize)
{
#ifdef ANDROID
    auto view = animationStates
                | std::views::transform(
                    [&](const std::pair<std::string, dgm::Clip>& pair)
                    {
                        return std::pair {
                            pair.first,
                            recomputeClip(pair.second, startCoord, textureSize),
                        };
                    });
    return dgm::AnimationStates(view.begin(), view.end());
#else
    return animationStates
           | std::views::transform(
               [&](const std::pair<std::string, dgm::Clip>& pair)
               {
                   return std::pair {
                       pair.first,
                       recomputeClip(pair.second, startCoord, textureSize),
                   };
               })
           | std::ranges::to<dgm::AnimationStates>();
#endif
}

void dgm::TextureAtlas::copyTexture(
    const sf::Texture& textureToCopy, const sf::Vector2i& offset)
{
    auto imageToCopy = textureToCopy.copyToImage();
    if (!atlasImage.copy(
            imageToCopy,
            sf::Vector2u(offset),
            sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0)), // full image
            /* applyAlpha */ false))
    {
        throw dgm::Exception("Could not copy source texture to atlas");
    }

    if (!atlasTexture.loadFromImage(atlasImage))
    {
        throw dgm::Exception("Could not create underlying atlas texture");
    }
}
