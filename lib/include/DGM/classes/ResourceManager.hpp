#pragma once

#include <DGM/classes/Compatibility.hpp>
#include <DGM/classes/Error.hpp>
#include <concepts>
#include <expected>
#include <filesystem>
#include <functional>
#include <map>
#include <string>

#ifdef ANDROID
#include <SFML/System/NativeActivity.hpp>
#include <android/native_activity.h>
#endif

namespace dgm
{
    template<class T>
    concept CompatibleResourceType = std::is_move_constructible_v<T>;

    /**
    ResourceManager allows for loading any kind of
    resource from a disk and only providing an API
    to get an immutable reference to it.

    Lifetime of all resources is limited by the lifetime
    of this manager object.
    */
    class [[nodiscard]] ResourceManager final
    {
    private:
        using byte = unsigned char;

    public:
        ResourceManager() noexcept = default;

        ResourceManager(ResourceManager&& other) noexcept;

        ResourceManager(const ResourceManager&) = delete;

        ~ResourceManager();

    public:
        /**
        Get a read-only reference to resource that has
        been previously loaded.

        If error occures, an error message is returned instead.
        */
        template<CompatibleResourceType T>
        const T& get(const std::string& id) const
        {
            if (!hasResource<T>(id))
                throw Error("Resource with id '" + id + "' is not loaded.");

            const auto&& tid = typeid(T).hash_code();
            auto&& ptr = data.at(tid).at(id);
            return std::cref(*reinterpret_cast<const T*>(ptr));
        }

        template<CompatibleResourceType T>
        T& getMutable(const std::string& id) const
        {
            if (!hasResource<T>(id))
                throw Error("Resource with id '" + id + "' is not loaded.");

            const auto&& tid = typeid(T).hash_code();
            auto&& ptr = data.at(tid).at(id);
            return std::ref(*reinterpret_cast<T*>(ptr));
        }

        /**
         Callback used to load a particular resource

         It accepts a path to the original resource on a disk
         and a reference to initialized variable that should
         be filled with the loaded data.

         Callback is expected to throw std::runtime_error if
         any error occurs.
         */
        template<CompatibleResourceType T>
        using LoadCallback = std::function<std::expected<T, Error>(
            const std::filesystem::path&)>;

        /**
        Loads resource into the database from the given path
        using the loadCallback.

        An ID will be assigned to the resource with
        \see getResourceId

        Function returns std::true_type on success and error message
        if anything fails (for example loadCallback throws an error)
        */
        template<CompatibleResourceType T>
        NODISCARD_RESULT ExpectedSuccess loadResource(
            const std::filesystem::path& path, LoadCallback<T> loadCallback)
        {
            const auto&& tid = typeid(T).hash_code();
            const auto&& resId = getResourceId(path);
            if (!resId.has_value()) return std::unexpected(resId.error());

            if (hasResource<T>(resId.value()))
            {
                return std::unexpected(
                    "Resource with id '" + resId.value()
                    + "' is already loaded in the manager.");
            }

            try
            {
                auto&& resource = loadCallback(path);
                if (!resource) return std::unexpected(resource.error());

                registerDestructor<T>(tid);
                data[tid][resId.value()] = allocateInitializedMemory<T>();
                new (data[tid][resId.value()]) T(std::move(resource.value()));
            }
            catch (const std::exception& e)
            {
                return std::unexpected(
                    std::string("Unable to load resource. Reason: ")
                    + e.what());
            }

            return std::true_type {};
        }

        /**
         *  Remove resource with a given id from the database
         *
         *  If no resource of given type or with given id is in
         *  the database, this function throws an exception.
         */
        template<CompatibleResourceType T>
        std::expected<std::true_type, dgm::Error>
        unloadResource(const std::string& id)
        {
            try
            {
                const auto&& tid = typeid(T).hash_code();
                auto&& resources = data.at(tid);
                auto&& itr = resources.find(id);
                if (itr == resources.end())
                    throw std::runtime_error("Id '" + id + "' is not loaded");

                destructors.at(tid)(itr->second);
                ::operator delete(itr->second);
                resources.erase(itr);
            }
            catch (std::exception& e)
            {
                return std::unexpected(
                    std::string("Unloading resource failed. Reason: ")
                    + e.what());
            }

            return std::true_type {};
        }

        /**
        Loads resources from a directory (non-recursive due to Android
        limitations).

        Only files that have the \p allowedExtensions will be
        loaded via provided \p loadCallback.

        Each file will have an id computed by \see getResourceId.
        You can use \see getLoadedResourceIds to list all ids that
        have been loaded.

        Function returns std::true_type on success and error message
        if anything fails (for example loadCallback throws an error)

        \warn \p allowedExtensions must not be empty!
        */
        template<CompatibleResourceType T>
        NODISCARD_RESULT ExpectedSuccess loadResourcesFromDirectory(
            const std::filesystem::path& folderPath,
            LoadCallback<T> loadCallback,
            const std::vector<std::string>& allowedExtensions = {})
        {
            namespace fs = std::filesystem;

            if (allowedExtensions.empty())
            {
                return std::unexpected(
                    dgm::Error("Allowed extensions must not be empty!"));
            }

            auto hasAllowedExtension =
                [&allowedExtensions](fs::path path) -> bool
            {
                return std::find_if(
                           allowedExtensions.begin(),
                           allowedExtensions.end(),
                           [&path](const std::string& extension) -> bool
                           {
                               return path.extension().string().ends_with(
                                   extension);
                           })
                       != allowedExtensions.end();
            };

#ifdef ANDROID
            auto& activity = *sf::getNativeActivity();
            auto dir = std::unique_ptr<AAssetDir, decltype(&AAssetDir_close)>(
                AAssetManager_openDir(
                    activity.assetManager, folderPath.string().c_str()),
                AAssetDir_close);
            if (!dir)
#else
            fs::path path(folderPath);
            if (!fs::is_directory(path))
#endif
            {
                return std::unexpected(
                    "Path '" + folderPath.string() + "' is not a directory!");
            }

#ifdef ANDROID
            while (auto item = AAssetDir_getNextFileName(dir.get()))
#else
            auto&& itr = fs::directory_iterator(path);
            for (auto item : itr)
#endif
            {
                auto&& itemPath = fs::path(item);

                if (hasAllowedExtension(itemPath))
                {
                    auto&& result = loadResource<T>(itemPath, loadCallback);
                    if (!result) return result;
                }
            }

            return std::true_type {};
        }

        NODISCARD_RESULT std::expected<std::string, Error>
        getResourceId(const std::filesystem::path& path) const noexcept;

        template<CompatibleResourceType T>
        NODISCARD_RESULT bool hasResource(const std::string& id) const noexcept
        {
            const auto&& tid = typeid(T).hash_code();
            return data.contains(tid) && data.at(tid).contains(id);
        }

        /**
        Returns a list of resource ids that have been loaded for
        a particular template type.

        If no resources has been loaded
        */
        template<CompatibleResourceType T>
        NODISCARD_RESULT std::expected<std::vector<std::string>, Error>
        getLoadedResourceIds() const noexcept
        {
            try
            {
                std::vector<std::string> result;
                const auto&& typeHash = typeid(T).hash_code();
                if (!data.contains(typeHash)) return result;

                for (auto&& [id, _] : data.at(typeHash))
                    result.push_back(id);

                return result;
            }
            catch (const std::exception& e)
            {
                return std::unexpected(
                    std::string("Cannot list all resource ids. Reason: ")
                    + e.what());
            }
        }

    private:
        template<class T>
        void registerDestructor(std::size_t typeHash)
        {
            if (destructors.count(typeHash) == 0)
            {
                destructors[typeHash] = [](byte* buffer)
                { reinterpret_cast<T*>(buffer)->~T(); };
            }
        }

        template<class T>
        byte* allocateInitializedMemory()
        {
            byte* buffer = static_cast<byte*>(::operator new(sizeof(T)));
            if (!buffer)
            {
                throw std::bad_alloc();
            }

            return buffer;
        }

    private:
        // Indexed by type_hash
        // Contains submap indexed by resource id
        // which contains byte arrays of individual resources
        std::unordered_map<std::size_t, std::unordered_map<std::string, byte*>>
            data = {};

        // Indexed by type_hash, contains
        // callbacks to each type destructor
        // for proper memory freeing
        std::unordered_map<std::size_t, std::function<void(byte*)>> destructors;
    };
} // namespace dgm
