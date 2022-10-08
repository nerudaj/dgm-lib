#include <DGM/classes/ResourceManager.hpp>

#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <algorithm>

using dgm::ResourceManager;
namespace fs = std::filesystem;

void ResourceManager::loadResourceFromFile(const std::string& filename, sf::Texture& texture)
{
	if (not texture.loadFromFile(filename))
	{
		throw std::runtime_error("Cannot load texture");
	}
}

void ResourceManager::loadResourceFromFile(const std::string& filename, sf::Font& font)
{
	if (not font.loadFromFile(filename))
	{
		throw std::runtime_error("Cannot load font");
	}
}

void ResourceManager::loadResourceFromFile(const std::string& filename, sf::SoundBuffer& sound)
{
	if (not sound.loadFromFile(filename))
	{
		throw std::runtime_error("Cannot load sound buffer");
	}
}

void ResourceManager::loadResourceFromFile(const std::string& filename, dgm::AnimationStates& states)
{
	states = loader.loadAnimationsFromFile(filename);
}

std::string dgm::ResourceManager::getResourceId(const std::string& filename)
{
	auto itr = filename.find_last_of('/');
	return filename.substr(++itr);
}

template<dgm::SupportedResourceType T>
void ResourceManager::loadResource(const std::string& filename)
{
	T* resource = new T;
	if (!resource)
	{
		throw dgm::EnvironmentException("Could not allocate memory for resource");
	}

	try
	{
		loadResourceFromFile(filename, *resource);
	}
	catch (std::exception& e)
	{
		delete resource;
		throw dgm::GeneralException(std::format(
			"Could not load resource '{}', reason: '{}'",
			filename,
			e.what()));
	}

	std::string id = getResourceId(filename);
	if (isResourceInDatabase(id))
	{
		delete resource;
		throw dgm::ResourceException(std::format(
			"Resource called '{}' is already in database!",
			id));
	}

	try
	{
		database[id] = resource;
	}
	catch (...)
	{
		delete resource;
		throw dgm::EnvironmentException("Could not insert resource into database");
	}
}

template void ResourceManager::loadResource<sf::Texture>(const std::string& filename);
template void ResourceManager::loadResource<sf::SoundBuffer>(const std::string& filename);
template void ResourceManager::loadResource<sf::Font>(const std::string& filename);
template void ResourceManager::loadResource<dgm::AnimationStates>(const std::string& filename);

template<dgm::SupportedResourceType T>
void ResourceManager::loadResourceDir(
	const std::string& folderPath,
	const std::vector<std::string>& allowedExtensions,
	bool recursive)
{
	if (allowedExtensions.empty())
	{
		throw dgm::ResourceException("Allowed extensions must not be empty!");
	}

	fs::path path(folderPath);
	if (not fs::is_directory(path))
	{
		throw dgm::ResourceException(std::format("Path '{}' does not exist!", folderPath));
	}

	fs::directory_iterator itr(path);
	for (auto item : itr)
	{
		fs::path itemPath(item);

		if (fs::is_directory(itemPath))
		{
			if (recursive)
				loadResourceDir<T>(itemPath.generic_string(), allowedExtensions, recursive);
			else
				continue;
		}

		if (std::find_if(
			allowedExtensions.begin(),
			allowedExtensions.end(),
			[&itemPath] (const std::string& extension) -> bool
			{
				return itemPath.extension().string().ends_with(extension);
			}) == allowedExtensions.end())
		{
			continue;
		}

			loadResource<T>(itemPath.generic_string());
	}
}

template void ResourceManager::loadResourceDir<sf::Texture>(
	const std::string& folderPath,
	const std::vector<std::string>& allowedExtensions,
	bool recursive);
template void ResourceManager::loadResourceDir<sf::SoundBuffer>(
	const std::string& folderPath,
	const std::vector<std::string>& allowedExtensions,
	bool recursive);
template void ResourceManager::loadResourceDir<sf::Font>(
	const std::string& folderPath,
	const std::vector<std::string>& allowedExtensions,
	bool recursive);
template void ResourceManager::loadResourceDir<dgm::AnimationStates>(
	const std::string& folderPath,
	const std::vector<std::string>& allowedExtensions,
	bool recursive);

ResourceManager::ResourceManager(ResourceManager&& other) noexcept : loader(other.loader)
{
	database = other.database;
	other.database.clear();
}

ResourceManager::~ResourceManager() noexcept
{
	for (auto&& [key, value] : database)
	{
		free(value);
	}
}