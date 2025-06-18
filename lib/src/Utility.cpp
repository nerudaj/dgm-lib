#include <DGM/classes/Utility.hpp>
#include <SFML/System/FileInputStream.hpp>

std::expected<std::string, dgm::Error>
dgm::Utility::loadFileAllText(const std::filesystem::path& path)
{
    sf::FileInputStream stream(path);
    if (!stream)
    {
        return std::unexpected(
            "Could not open file '" + path.string() + "' for reading");
    }

    return dgm::Utility::loadFileAllText(stream);
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
