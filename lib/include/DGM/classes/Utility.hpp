#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/Traits.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <fstream>
#include <string>

namespace dgm
{

    class Utility
    {
    public:
        template<SfmlVectorType T>
        static std::string to_string(const T& vec)
        {
            return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y)
                   + "]";
        }

        template<SfmlVectorType T>
        struct less
        {
            [[nodiscard]] constexpr inline bool
            operator()(const T& a, const T& b) const noexcept
            {
                return a.y == b.y ? a.x < b.x : a.y < b.y;
            }
        };

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static std::expected<sf::Texture, dgm::Error>
        loadTexture(const std::filesystem::path& path)
        {
            auto&& texture = sf::Texture();
            if (!texture.loadFromFile(path.string()))
            {
                return std::unexpected(
                    "Could not load texture from path '" + path.string() + "'");
            }
            return texture;
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static std::expected<sf::Font, dgm::Error>
        loadFont(const std::filesystem::path& path)
        {
            auto&& font = sf::Font();
            if (!font.openFromFile(path.string()))
            {
                return std::unexpected(
                    "Could not load font from path '" + path.string() + "'");
            }
            return font;
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static std::expected<sf::SoundBuffer, dgm::Error>
        loadSound(const std::filesystem::path& path)
        {
            auto&& sound = sf::SoundBuffer();
            if (!sound.loadFromFile(path.string()))
            {
                return std::unexpected(
                    "Could not load sound from path '" + path.string() + "'");
            }
            return sound;
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static std::expected<dgm::Clip, dgm::Error>
        loadClip(const std::filesystem::path& path)
        {
            try
            {
                return dgm::JsonLoader().loadClipFromFile(path);
            }
            catch (const std::exception& e)
            {
                return std::unexpected(
                    "Could not load dgm::Clip from file '" + path.string()
                    + "', reason: " + e.what());
            }
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static std::expected<dgm::AnimationStates, dgm::Error>
        loadAnimationStates(const std::filesystem::path& path)
        {
            try
            {
                return dgm::JsonLoader().loadAnimationsFromFile(path);
            }
            catch (const std::exception& e)
            {
                return std::unexpected(
                    "Could not load dgm::AnimationStates from file '"
                    + path.string() + "', reason: " + e.what());
            }
        }

        /**
         * \brief Platform-independent helper for loading an asset file into a
         * string
         *
         * \param path The path to the file to load
         *
         * If you develop for Android, use this function to load any file
         * that is bundled inside the APK as an asset. These files don't sit
         * in a regular filesystem, so they require a different treatment.
         *
         * For files placed inside internal/external storage, use
         * loadFileAllText. On Windows, you can safely intermix this method and
         * loadFileAllText.
         *
         * \return std::string on success, dgm::Error on failure
         */
        static std::expected<std::string, dgm::Error>
        loadAssetAllText(const std::filesystem::path& path);

        /**
         * \brief Platform-independent helper for loading file into a string
         *
         * \param stream The stream to read from
         *
         * If you develop for Android, use this function to load any file
         * that is bundled inside the APK as an asset. These files don't sit
         * in a regular filesystem, so they require a different treatment.
         *
         * For files placed inside internal/external storage, use
         * loadFileAllText. On Windows, you can safely intermix this method and
         * loadFileAllText.
         *
         * \return std::string on success, dgm::Error on failure
         */
        static std::expected<std::string, dgm::Error>
        loadAssetAllText(sf::InputStream& stream);

        /**
         * \brief Platform-independent helper for loading an asset file into a
         * string
         *
         * \param path The path to the file to load
         *
         * If you develop for Android, use this function for loading files
         * located inside internal/external storage. For loading files packaged
         * as assets inside the APK itself, use loadAssetAllText. On Windows,
         * you can safely use whichever.
         *
         * \return std::string on success, dgm::Error on failure
         */
        static std::expected<std::string, dgm::Error>
        loadFileAllText(const std::filesystem::path& path);

        /**
         * \brief Platform-independent helper for loading file into a string
         *
         * \param stream The stream to read from
         *
         * If you develop for Android, use this function for loading files
         * located inside internal/external storage. For loading files packaged
         * as assets inside the APK itself, use loadAssetAllText. On Windows,
         * you can safely use whichever.
         *
         * \return std::string on success, dgm::Error on failure
         */
        static std::expected<std::string, dgm::Error>
        loadFileAllText(std::istream& stream);
    };
} // namespace dgm
