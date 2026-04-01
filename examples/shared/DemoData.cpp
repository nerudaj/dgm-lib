#include "DemoData.hpp"

Level DemoData::createDemoLevel(
    const sf::Texture& texture, const dgm::Clip& clip)
{
    const auto meshSize = sf::Vector2u(15, 15);
    const auto voxelSize = sf::Vector2u(64, 64);

    // clang-format off
    return Level(
        dgm::Mesh(std::vector<int>{
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 
            1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        }, meshSize, voxelSize),
        texture,
        clip,
        dgm::Mesh(std::vector<int>{
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 
            1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        }, meshSize, voxelSize)
    );
    // clang-format on
}

Level DemoData::createDemoLevel2(
    const sf::Texture& texture, const dgm::Clip& clip)
{
    const auto meshSize = sf::Vector2u(15, 10);
    const auto voxelSize = sf::Vector2u(64, 64);

    // clang-format off
    return Level(
        dgm::Mesh(std::vector<int>{
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 3, 0, 0, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        }, meshSize, voxelSize),
        texture,
        clip,
        dgm::Mesh(std::vector<int>{
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 
            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 
            1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        }, meshSize, voxelSize)
    );
    // clang-format on
}

#define CHECK_RESULT(x)                                                        \
    if (auto result = x; !result)                                              \
        throw std::runtime_error(result.error().getMessage());

dgm::ResourceManager DemoData::loadDemoResources()
{
    dgm::ResourceManager resmgr;
    CHECK_RESULT(resmgr.loadResourcesFromDirectory<sf::Font>(
        RESOURCE_DIR, dgm::Utility::loadFont, { ".ttf" }));
    CHECK_RESULT(resmgr.loadResourcesFromDirectory<sf::Texture>(
        RESOURCE_DIR, dgm::Utility::loadTexture, { ".png" }));
    CHECK_RESULT(resmgr.loadResourcesFromDirectory<dgm::AnimationStates>(
        RESOURCE_DIR, dgm::Utility::loadAnimationStates, { ".json" }));
    CHECK_RESULT(resmgr.loadResourcesFromDirectory<dgm::Clip>(
        RESOURCE_DIR, dgm::Utility::loadClip, { ".clip" }));
    return resmgr;
}

dgm::DynamicBuffer<dgm::Rect> DemoData::spawnCherries()
{
    const std::vector<sf::Vector2f> positions = {
        { 96.f, 512.f },  { 363.f, 290.f }, { 668.f, 384.f }, { 839.f, 734.f },
        { 542.f, 622.f }, { 393.f, 778.f }, { 159.f, 757.f }, { 287.f, 310.f },
    };
    const sf::Vector2f size = { 16.f, 16.f };

    auto&& cherries = dgm::DynamicBuffer<dgm::Rect>(16);

    for (auto&& position : positions)
        cherries.emplaceBack(position, size);

    return cherries;
}
