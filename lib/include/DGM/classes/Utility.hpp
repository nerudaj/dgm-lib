#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/Traits.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <iostream>
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
            NODISCARD_RESULT constexpr inline bool
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
    };
} // namespace dgm
