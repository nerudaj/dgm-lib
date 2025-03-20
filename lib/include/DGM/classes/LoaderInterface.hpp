#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Compatibility.hpp>
#include <filesystem>
#include <istream>

namespace dgm
{
    /**
     *  Interface class for a data loader
     *
     *  Currently used by ResourceManager so you can define your own format for
     * storing certain data structures.
     */
    class LoaderInterface
    {
    public:
        NODISCARD_RESULT virtual dgm::Clip
        loadClipFromFile(const std::filesystem::path&) const = 0;

        NODISCARD_RESULT virtual dgm::AnimationStates
        loadAnimationsFromFile(const std::filesystem::path&) const = 0;

        virtual ~LoaderInterface() = default;
    };

} // namespace dgm