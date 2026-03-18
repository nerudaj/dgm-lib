#include <DGM/classes/ResourceManager.hpp>
#include <stdexcept>

using dgm::Error;
using dgm::ResourceManager;
namespace fs = std::filesystem;

ResourceManager::ResourceManager(ResourceManager&& other) noexcept
    : data(std::move(other.data)), destructors(std::move(other.destructors))
{
}

ResourceManager::~ResourceManager()
{
    for (auto&& [typeId, hashmap] : data)
        for (auto&& [_, resourceBytes] : hashmap)
        {
            destructors.at(typeId)(resourceBytes);
            ::operator delete(resourceBytes);
        }
}

std::expected<std::string, Error>
dgm::ResourceManager::getResourceId(const std::filesystem::path& path) noexcept
{
    try
    {
        auto&& id = path.filename().string();
        if (id.empty()) throw std::runtime_error("Path is empty");
        return id;
    }
    catch (std::exception& e)
    {
        return std::unexpected(Error(
            std::string("Cannot compute resource id. Reason: ") + e.what()));
    }
}
