#include <DGM/classes/Error.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/Utility.hpp>
#include <SFML/System/FileInputStream.hpp>
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
    return sf::IntRect {
        sf::Vector2i {
            jsonBounds["left"].get<int>(),
            jsonBounds["top"].get<int>(),
        },
        sf::Vector2i {
            jsonBounds["width"].get<int>(),
            jsonBounds["height"].get<int>(),
        },
    };
}

dgm::Clip
dgm::JsonLoader::loadClipFromFile(const std::filesystem::path& filename) const
{
    sf::FileInputStream load(filename);
    return loadClipFromStream(load);
}

dgm::Clip dgm::JsonLoader::loadClipFromStream(sf::InputStream& stream) const
{
    const auto&& fileText = dgm::Utility::loadFileAllText(stream);
    if (!fileText) throw dgm::Exception(fileText.error().getMessage());
    auto&& json = nlohmann::json::parse(fileText.value());

    const auto&& frame = getFrameFromJson(json["frame"]);
    const auto&& bounds = getBoundsFromJson(json["bounds"]);
    const auto&& spacing = json.contains("spacing")
                               ? getSpacingFromJson(json["spacing"])
                               : sf::Vector2u(0u, 0u);
    const auto&& nframes =
        json.contains("nframes") ? json["nframes"].get<unsigned>() : 0;

    return dgm::Clip(frame, bounds, nframes, spacing);
}

dgm::AnimationStates dgm::JsonLoader::loadAnimationsFromFile(
    const std::filesystem::path& filename) const
{
    sf::FileInputStream load(filename);
    return loadAnimationsFromStream(load);
}

dgm::AnimationStates
dgm::JsonLoader::loadAnimationsFromStream(sf::InputStream& stream) const
{
    const auto&& fileText = dgm::Utility::loadFileAllText(stream);
    if (!fileText) throw dgm::Exception(fileText.error().getMessage());
    auto&& file = nlohmann::json::parse(fileText.value());

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
        const auto& name = state["name"];
        const auto&& frame = state.contains("frame")
                                 ? getFrameFromJson(state["frame"])
                                 : defaultFrame;
        const auto&& spacing = state.contains("spacing")
                                   ? getSpacingFromJson(state["spacing"])
                                   : defaultSpacing;
        const auto&& frameCount =
            state.contains("nframes") ? state["nframes"].get<unsigned>() : 0;
        const auto&& bounds = getBoundsFromJson(state["bounds"]);

        if (frame.x == 0u || frame.y == 0u)
        {
            throw dgm::Exception(
                "No frame was defined either in defaults block or for state "
                + name);
        }

        result.insert({ name, dgm::Clip(frame, bounds, frameCount, spacing) });
    }

    return result;
}
