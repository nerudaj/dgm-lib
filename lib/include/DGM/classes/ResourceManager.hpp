#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/LoaderInterface.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <map>

namespace dgm
{
	template<class T>
	concept SupportedResourceType =
		std::is_same_v<T, sf::Texture> ||
		std::is_same_v<T, sf::Font> ||
		std::is_same_v<T, sf::SoundBuffer> ||
		std::is_same_v<T, dgm::AnimationStates>;

	/**
	 *  \brief Class for managing resources
	 */
	class ResourceManager final
	{
	private:
		dgm::LoaderInterface& loader;
		std::map<std::string, void*> database = {};
		std::string commonPrefix = "";

	private:
		void loadResourceFromFile(const std::string& filename, sf::Texture& texture);
		void loadResourceFromFile(const std::string& filename, sf::Font& font);
		void loadResourceFromFile(const std::string& filename, sf::SoundBuffer& sound);
		void loadResourceFromFile(const std::string& filename, dgm::AnimationStates& states);

	public:
		/**
		 *  \brief Test whether resource with certain id is in the database
		 */
		bool isResourceInDatabase(const std::string& id) const
		{
			return database.find(id) != database.end();
		}

		/**
		 *  \brief Get resource ID based on its filename
		 */
		std::string getResourceId(const std::string& filePath);

		/**
		 *  \brief Get handle of a resource
		 *
		 *  \param[in]  id  Id of the resource
		 *
		 *  \see getResourceId
		 */
		template<SupportedResourceType T>
		T& get(const std::string& id) const
		{
			if (not isResourceInDatabase(id))
			{
				throw dgm::GeneralException("Resource '" + id + "' is not in database.");
			}

			return dynamic_cast<T&>(*((T*) (database.at(id))));
		}

		/**
		 *  \brief Load resource of specified type
		 *
		 *  \param[in]  filePath  Path to resource
		 */
		template<SupportedResourceType T>
		void loadResource(const std::string& filePath);

		/**
		 *  \brief Load directory full of resources of specified type
		 *
		 *  \param[in]  folderPath         Path to directory
		 *  \param[in]  allowedExtensions  Extensions of files that will be opened for loading. Other files will be ignored
		 *  \param[in]  recursive          Whether load subdirectories (Default: false)
		 */
		template<SupportedResourceType T>
		void loadResourceDir(
			const std::string& folderPath,
			const std::vector<std::string>& allowedExtensions,
			bool recursive = false);

		ResourceManager(dgm::LoaderInterface& loader) noexcept : loader(loader) {}
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager(ResourceManager&& other) noexcept;
		~ResourceManager() noexcept;
	};
}