#include <DGM/classes/Utility.hpp>
#include <SFML/System/FileInputStream.hpp>

std::expected<std::string, dgm::Error>
dgm::Utility::loadAssetAllText(const std::filesystem::path& path)
{
    try
    {
        sf::FileInputStream stream(path);
        return dgm::Utility::loadAssetAllText(stream);
    }
    catch (const std::exception& ex)
    {
        return std::unexpected(
            "Could not open file '" + path.string()
            + "' for reading: " + ex.what());
    }
}

std::expected<std::string, dgm::Error>
dgm::Utility::loadAssetAllText(sf::InputStream& stream)
{
    auto size = stream.getSize();
    if (!size) return std::unexpected(dgm::Error("Cannot get stream size"));

    std::string buffer(*size, '\0');
    auto readBytes = stream.read(buffer.data(), *size);

    if (!readBytes)
    {
        return std::unexpected(dgm::Error("Error while reading stream"));
    }

    if (*readBytes != *size)
    {
        return std::unexpected(
            dgm::Error("Could not read requested amount of bytes"));
    }

    return buffer;
}

std::expected<std::string, dgm::Error>
dgm::Utility::loadFileAllText(const std::filesystem::path& path)
{
    try
    {
        std::ifstream load(path);
        return loadFileAllText(load);
    }
    catch (const std::exception& e)
    {
        return std::unexpected(dgm::Error(e.what()));
    }
}

std::expected<std::string, dgm::Error>
dgm::Utility::loadFileAllText(std::istream& stream)
{
    stream.seekg(0, std::ios::end);
    size_t len = stream.tellg();
    stream.seekg(0, std::ios::beg);

    auto&& result = std::string(len, '\0');
    stream.read(result.data(), len);

    return result;
}
