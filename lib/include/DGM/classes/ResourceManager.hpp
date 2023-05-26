#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/LoaderInterface.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <expected>
#include <filesystem>
#include <functional>
#include <map>
#include <utility>

namespace dgm
{
    template<class T>
    concept CompatibleResourceType = std::is_constructible_v<T>;

    class ResourceManager final
    {
    private:
        using byte = unsigned char;

    public:
        [[nodiscard]] ResourceManager() noexcept = default;

        [[nodiscard]] ResourceManager(ResourceManager&& other) noexcept;

        ResourceManager(const ResourceManager&) = delete;

        ~ResourceManager() noexcept;

    public:
        /**
         *  Callback used to load a particular resource
         *
         *  It accepts a path to the original resource on a disk
         *  and a reference to initialized variable that should
         *  be filled with the loaded data.
         *
         *  Callback is expected to throw std::runtime_error if any error
         *  occurs.
         */
        template<CompatibleResourceType T>
        using LoadCallback =
            std::function<void(const std::filesystem::path&, T&)>;

        /**
         *  Get a read-only reference to resource that has been previously
         *  loaded.
         *
         *  If error occures, an error message is returned instead.
         */
        template<CompatibleResourceType T>
        [[nodiscard]] std::
            expected<std::reference_wrapper<const T>, ErrorMessage>
            get(const std::string& id) const noexcept
        {
            if (!hasResource<T>(id))
                return std::unexpected(
                    std::format("Resource with id {} is not loaded.", id));

            const auto&& tid = typeid(T).hash_code();
            auto&& ptr = data.at(tid).at(id);
            return std::cref(*reinterpret_cast<const T*>(ptr));
        }

        /**
         *  Loads resource into the database from the given path
         *  using the loadCallback.
         *
         *  An ID will be assigned to the resource with \see getResourceId
         *
         *  Function returns std::true_type on success and error message
         *  if anything fails (for example loadCallback throws an error)
         */
        template<CompatibleResourceType T>
        [[nodiscard]] ExpectedSuccess loadResource(
            const std::filesystem::path& path,
            LoadCallback<T> loadCallback) noexcept
        {
            const auto&& tid = typeid(T).hash_code();
            const auto&& resId = getResourceId(path);
            if (!resId.has_value()) return std::unexpected(resId.error());

            if (hasResource<T>(resId.value()))
            {
                return std::unexpected(std::format(
                    "Resource with id {} is already loaded in the manager.",
                    resId.value()));
            }

            try
            {
                registerDestructor<T>(tid);
                data[tid][resId.value()] = allocateInitializedMemory<T>();
                auto&& resourcePtr =
                    reinterpret_cast<T*>(data.at(tid).at(resId.value()));
                loadCallback(path, *resourcePtr);
            }
            catch (const std::exception& e)
            {
                return std::unexpected(std::format(
                    "Unable to load resource. Reason: {}", e.what()));
            }

            return std::true_type {};
        }

        /**
         *  Recursively loads resources from a directory.
         *
         *  Only files that have the \p allowedExtensions will be
         *  loaded via provided \p loadCallback.
         *
         *  Each file will have an id computed by \see getResourceId.
         *  You can use \see getLoadedResourceIds to list all ids that
         *  have been loaded.
         *
         *  Function returns std::true_type on success and error message
         *  if anything fails (for example loadCallback throws an error)
         *
         *  \warn \p allowedExtensions must not be empty!
         */
        template<CompatibleResourceType T>
        [[nodiscard]] ExpectedSuccess loadResourcesFromDirectory(
            const std::filesystem::path& folderPath,
            LoadCallback<T> loadCallback,
            const std::vector<std::string>& allowedExtensions = {}) noexcept
        {
            namespace fs = std::filesystem;

            if (allowedExtensions.empty())
            {
                return std::unexpected("Allowed extensions must not be empty!");
            }

            fs::path path(folderPath);
            if (!fs::is_directory(path))
            {
                return std::unexpected(std::format(
                    "Path '{}' is not a directory!", folderPath.string()));
            }

            auto hasAllowedExtension =
                [&allowedExtensions](fs::path path) -> bool
            {
                return std::find_if(
                           allowedExtensions.begin(),
                           allowedExtensions.end(),
                           [&path](const std::string& extension) -> bool {
                               return path.extension().string().ends_with(
                                   extension);
                           })
                       != allowedExtensions.end();
            };

            auto&& itr = fs::recursive_directory_iterator(path);
            for (auto item : itr)
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

        [[nodiscard]] std::expected<std::string, ErrorMessage>
        getResourceId(const std::filesystem::path& path) const noexcept;

        template<CompatibleResourceType T>
        [[nodiscard]] bool hasResource(const std::string& id) const noexcept
        {
            const auto&& tid = typeid(T).hash_code();
            return data.contains(tid) && data.at(tid).contains(id);
        }

        /**
         *  Returns a list of resource ids that have been loaded for
         *  a particular template type.
         *
         *  If no resources has been loaded
         */
        template<CompatibleResourceType T>
        [[nodiscard]] std::expected<std::vector<std::string>, ErrorMessage>
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
                return std::unexpected(std::format(
                    "Cannot list all resource ids. Reason: {}", e.what()));
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

            new (buffer) T();
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
