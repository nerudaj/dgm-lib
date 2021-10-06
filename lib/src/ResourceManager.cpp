#include <DGM/classes/ResourceManager.hpp>

#include <stdexcept>
#include <filesystem>

using dgm::ResourceManager;
namespace fs = std::filesystem;

void ResourceManager::loadResourceFromFile(const std::string &filename, sf::Texture &texture) {
	if (not texture.loadFromFile(filename)) {
		throw std::runtime_error("Cannot load texture");
	}
}

void ResourceManager::loadResourceFromFile(const std::string &filename, sf::Font &font) {
	if (not font.loadFromFile(filename)) {
		throw std::runtime_error("Cannot load font");
	}
}

void ResourceManager::loadResourceFromFile(const std::string &filename, sf::SoundBuffer &sound) {
	if (not sound.loadFromFile(filename)) {
		throw std::runtime_error("Cannot load sound buffer");
	}
}

void ResourceManager::loadResourceFromFile(const std::string &filename, std::shared_ptr<dgm::AnimationStates> &states) {
	states = dgm::Animation::loadStatesFromFile(filename);
}

std::string dgm::ResourceManager::getResourceName(const std::string & filename) {
	auto itr = filename.find_last_of('/');
	return filename.substr(++itr);
}

template<typename T>
void ResourceManager::loadResource(const std::string &filename) {
	T *resource = new T;
	if (!resource) {
		throw dgm::EnvironmentException("Could not allocate memory for resource");
	}
	
	try {
		loadResourceFromFile(filename, *resource);
	}
	catch (std::exception &e) {
		delete resource;
		throw dgm::GeneralException("Could not load resource '" + filename + "', reason: '" + std::string(e.what()) + "'");
	}

	std::string name = getResourceName(filename);
	if (isResourceInDatabase(name)) {
		delete resource;
		throw dgm::ResourceException("Resource called '" + name + "' is already in database!");
	}

	try {
		database[name] = resource;
	}
	catch (...) {
		delete resource;
		throw dgm::EnvironmentException("Could not insert resource into database");
	}
}

template void ResourceManager::loadResource<sf::Texture>(const std::string &filename);
template void ResourceManager::loadResource<sf::SoundBuffer>(const std::string &filename);
template void ResourceManager::loadResource<sf::Font>(const std::string &filename);
template void ResourceManager::loadResource<std::shared_ptr<dgm::AnimationStates>>(const std::string &filename);

template<typename T>
void ResourceManager::loadResourceDir(const std::string &folder, bool recursive) {
	fs::path path(folder);
	if (not fs::is_directory(path)) {
		throw dgm::ResourceException("Path '" + folder + "' does not exist!");
	}

	fs::directory_iterator itr(path);
	for (auto item : itr) {
		fs::path itemPath(item);

		if (fs::is_directory(itemPath) && recursive) {
			loadResourceDir<T>(itemPath.generic_string(), recursive);
		}

		try {
			// FIX: Skip Thumbs.db file on Windows
			if (itemPath.generic_string() == "Thumbs.db") continue;

			loadResource<T>(itemPath.generic_string());
		}
		catch (dgm::GeneralException &e) {
			if (pedantic) throw e;
		}
	}
}

template void ResourceManager::loadResourceDir<sf::Texture>(const std::string &filename, bool recursive);
template void ResourceManager::loadResourceDir<sf::SoundBuffer>(const std::string &filename, bool recursive);
template void ResourceManager::loadResourceDir<sf::Font>(const std::string &filename, bool recursive);
template void ResourceManager::loadResourceDir<std::shared_ptr<dgm::AnimationStates>>(const std::string &filename, bool recursive);

ResourceManager::ResourceManager(ResourceManager &&other) {
	database = other.database;
	other.database.clear();
}

ResourceManager::~ResourceManager() {
	for (auto&& [key, value] : database) {
		free(value);
	}
}