#include <DGM/classes/Error.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

sf::Vector2u getFrameFromJson(const nlohmann::json& jsonFrame)
{
    return sf::Vector2u(
        jsonFrame["width"].get<unsigned>(),
        jsonFrame["height"].get<unsigned>());
}

sf::Vector2u getSpacingFromJson(const nlohmann::json& jsonFrame)
{
    return sf::Vector2u(
        jsonFrame["horizontal"].get<unsigned>(),
        jsonFrame["vertical"].get<unsigned>());
}

sf::IntRect getBoundsFromJson(const nlohmann::json& jsonBounds)
{
    return sf::IntRect(
        jsonBounds["left"].get<int>(),
        jsonBounds["top"].get<int>(),
        jsonBounds["width"].get<int>(),
        jsonBounds["height"].get<int>());
}

dgm::Clip
dgm::JsonLoader::loadClipFromFile(const std::filesystem::path& filename) const
{
    std::ifstream load(filename);
    return loadClipFromStream(load);
}

dgm::Clip dgm::JsonLoader::loadClipFromStream(std::istream& stream) const
{
    nlohmann::json json;
    stream >> json;

    const sf::Vector2u frame = getFrameFromJson(json["frame"]);
    const sf::IntRect bounds = getBoundsFromJson(json["bounds"]);
    const sf::Vector2u spacing = json.contains("spacing")
                                     ? getSpacingFromJson(json["spacing"])
                                     : sf::Vector2u(0u, 0u);
    const unsigned nframes =
        json.contains("nframes") ? json["nframes"].get<unsigned>() : 0;

    return dgm::Clip(frame, bounds, nframes, spacing);
}

dgm::AnimationStates dgm::JsonLoader::loadAnimationsFromFile(
    const std::filesystem::path& filename) const
{
    std::ifstream load(filename);
    return loadAnimationsFromStream(load);
}

dgm::AnimationStates
dgm::JsonLoader::loadAnimationsFromStream(std::istream& stream) const
{
    nlohmann::json file;
    stream >> file;

    AnimationStates result;

    // Parse defaults from file
    sf::Vector2u defaultFrame = { 0, 0 };
    sf::Vector2u defaultSpacing = { 0, 0 };
    if (file.contains("defaults"))
    {
        if (file["defaults"].contains("frame"))
        {
            defaultFrame = getFrameFromJson(file["defaults"]["frame"]);
        }

        if (file["defaults"].contains("spacing"))
        {
            defaultSpacing = getSpacingFromJson(file["defaults"]["spacing"]);
        }
    }

    // Parse through states
    for (auto&& state : file["states"])
    {
        const std::string name = state["name"];
        const sf::Vector2u frame = state.contains("frame")
                                       ? getFrameFromJson(state["frame"])
                                       : defaultFrame;
        const sf::Vector2u spacing = state.contains("spacing")
                                         ? getSpacingFromJson(state["spacing"])
                                         : defaultSpacing;
        const unsigned frameCount =
            state.contains("nframes") ? state["nframes"].get<unsigned>() : 0;
        const sf::IntRect bounds = getBoundsFromJson(state["bounds"]);

        if (frame.x == 0u || frame.y == 0u)
        {
            throw dgm::Exception(std::format(
                "No frame was defined either in defaults block or for state {}",
                name));
        }

        result.insert({ name, dgm::Clip(frame, bounds, frameCount, spacing) });
    }

    return result;
}
