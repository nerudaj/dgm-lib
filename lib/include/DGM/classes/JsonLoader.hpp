#pragma once

#include <DGM/classes/LoaderInterface.hpp>
#include <SFML/System/InputStream.hpp>
#include <filesystem>

namespace dgm
{

    /**
     *  Class that can load certain dgm structures from
     *  JSON files or streams.
     */
    class JsonLoader final : public LoaderInterface
    {
    public:
        /**
         *  Required format of JSON:
         {
                "frame": {
                        "width": 16,
                        "height": 16
                },
                "bounds": {
                        "left": 10,
                        "top": 10,
                        "width": 52,
                        "height": 52
                },
                "spacing": {
                        "horizontal": 10,
                        "vertical": 10
                },
                "nframes": 3
         }
         *
         * spacing and nframes are optional properties.
         * spacing determines space between frames, while
         * nframes limits total number of frames. If not
         * specified, then spacing is [0, 0] and nframes
         * is maximum avaialable in regards to frame size
         * and boundaries.
         */
        NODISCARD_RESULT virtual dgm::Clip
        loadClipFromFile(const std::filesystem::path&) const override;

        NODISCARD_RESULT dgm::Clip loadClipFromStream(sf::InputStream&) const;

        /**
         * Required format of JSON:
         {
                "defaults": {
                        "frame": {
                                "width": 16,
                                "height": 16
                        },
                        "spacing": {
                                "horizontal": 10,
                                "vertical": 10
                        }
                },
                "states": [
                        {
                                "name": "idle",
                                "bounds": {
                                        "left": 10,
                                        "top": 10,
                                        "width": 52,
                                        "height": 52
                                },
                                "nframes": 3
                        },
                        {
                                "name": "idle2",
                                "frame": {
                                        "width": 8,
                                        "height": 8
                                },
                                "spacing": {
                                        "horizontal": 0,
                                        "vertical": 0
                                },
                                "bounds": {
                                        "left": 0,
                                        "top": 52,
                                        "width": 16,
                                        "height": 32
                                },
                                "nframes": 7
                        }
                ]
         }

         * Format is similar to dgm::Clip, except animation
         * is comprised from many "states", each state having
         * its own clip.
         *
         * One can use the defaults key to specify common
         * frame size and common spacing.
         */
        NODISCARD_RESULT virtual dgm::AnimationStates
        loadAnimationsFromFile(const std::filesystem::path&) const override;

        NODISCARD_RESULT dgm::AnimationStates
        loadAnimationsFromStream(sf::InputStream&) const;
    };
} // namespace dgm