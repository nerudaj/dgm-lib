#pragma once

#include <DGM\dgm.hpp>
#include <map>

namespace dgm {
	/**
	 *  \brief Class for managing resources
	 */
	class ResourceManager {
	private:
		bool pedantic;

		std::map<std::string, void*> database;
		std::string commonPrefix;

		void loadResourceFromFile(const std::string &filename, sf::Texture &texture);
		void loadResourceFromFile(const std::string &filename, sf::Font &font);
		void loadResourceFromFile(const std::string &filename, sf::SoundBuffer &sound);
		void loadResourceFromFile(const std::string &filename, std::shared_ptr<dgm::AnimationStates> &states);

	public:
		/**
		 *  \brief Test whether resource with certain id is in the database
		 */
		bool isResourceInDatabase(const std::string &id) const {
			return database.find(id) != database.end();
		}

		/**
		 *  \brief Set pedantic mode
		 * 
		 *  \note Set this option prior loading resources. It is set to TRUE by default
		 * 
		 *  When pedantic mode is disabled (it is enabled by default), the loadResourceDir
		 *  will ignore errors when loading of a specific resource fails (other errors, as 
		 *  conflicting ids or failed mallocs are still reported).
		 *  This case might happen for example when you try to load a directory of textures
		 *  and you have some readme text files in there. With pedantic set to false, those
		 *  readme files will effectively be ignored.
		 */
		void setPedantic(bool enable) {
			pedantic = enable;
		}

		/**
		 *  \brief Get resource ID based on its filename
		 */
		std::string getResourceName(const std::string &filename);

		/**
		 *  \brief Get handle of a resource
		 * 
		 *  \param[in]  id  Id of the resource
		 * 
		 *  Id of the resource is currently its name on disk, including extension.
		 *  Supported types for this function:
		 *   * sf::Texture
		 *   * sf::SoundBuffer
		 *   * sf::Font
		 *   * std::shared_ptr<dgm::AnimationStates>
		 */
		template<typename T>
		T &get(const std::string &id) const {
			if (not isResourceInDatabase(id)) {
				throw dgm::GeneralException("Resource '" + id + "' is not in database.");
			}

			return dynamic_cast<T&>(*((T*)(database.at(id))));
		}

		/**
		 *  \brief Load resource of specified type
		 * 
		 *  \param[in]  filename  Path to resource
		 * 
		 *  Supported types for this function:
		 *   * sf::Texture
		 *   * sf::SoundBuffer
		 *   * sf::Font
		 *   * std::shared_ptr<dgm::AnimationStates>
		 */
		template<typename T>
		void loadResource(const std::string &filename);

		/**
		 *  \brief Load directory full of resources of specified type
		 * 
		 *  \param[in]  folder     Path to directory
		 *  \param[in]  recursive  Whether load subdirectories (Default: false)
		 * 
		 *  Supported types for this function:
		 *   * sf::Texture
		 *   * sf::SoundBuffer
		 *   * sf::Font
		 *   * std::shared_ptr<dgm::AnimationStates>
		 * 
		 *  \see setPedantic
		 */
		template<typename T>
		void loadResourceDir(const std::string &folder, bool recursive = false);

		ResourceManager();
		ResourceManager(const ResourceManager &other) = delete;
		ResourceManager(ResourceManager &&other);
		~ResourceManager();
	};
}