#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/Traits.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <format>
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
            [[nodiscard]] constexpr inline bool
            operator()(const T& a, const T& b) const noexcept
            {
                return a.y == b.y ? a.x < b.x : a.y < b.y;
            }
        };

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static void
        loadTexture(const std::filesystem::path& path, sf::Texture& texture)
        {
            if (!texture.loadFromFile(path.string()))
            {
                throw std::runtime_error(std::format(
                    "Could not load texture from path '{}'", path.string()));
            }
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static void loadFont(const std::filesystem::path& path, sf::Font& font)
        {
            if (!font.loadFromFile(path.string()))
            {
                throw std::runtime_error(std::format(
                    "Could not load font from path '{}'", path.string()));
            }
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static void
        loadSound(const std::filesystem::path& path, sf::SoundBuffer& sound)
        {
            if (!sound.loadFromFile(path.string()))
            {
                throw std::runtime_error(std::format(
                    "Could not load sound from path '{}'", path.string()));
            }
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static void loadClip(const std::filesystem::path& path, dgm::Clip& clip)
        {
            clip = dgm::JsonLoader().loadClipFromFile(path);
        }

        /*
         * Use this as a LoadCallback for dgm::ResourceManager
         */
        static void loadAnimationStates(
            const std::filesystem::path& path, dgm::AnimationStates& states)
        {
            states = dgm::JsonLoader().loadAnimationsFromFile(path);
        }
    };
} // namespace dgm
