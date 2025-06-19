#include <DGM/classes/Utility.hpp>
#include <SFML/System/FileInputStream.hpp>

std::expected<std::string, dgm::Error>
dgm::Utility::loadFileAllText(const std::filesystem::path& path)
{
    try
    {
        sf::FileInputStream stream(path);
        return dgm::Utility::loadFileAllText(stream);
    }
    catch (const std::exception& ex)
    {
        return std::unexpected(
            "Could not open file '" + path.string()
            + "' for reading: " + ex.what());
    }
}

std::expected<std::string, dgm::Error>
dgm::Utility::loadFileAllText(sf::InputStream& stream)
{
    auto size = stream.getSize();
    if (!size) throw dgm::Error("Cannot get stream size");

    std::string buffer(*size, '\0');
    auto readBytes = stream.read(buffer.data(), *size);

    if (!readBytes)
    {
        throw dgm::Error("Error while reading stream");
    }

    if (*readBytes != *size)
    {
        throw dgm::Error("Could not read requested amount of bytes");
    }

    return buffer;
}
