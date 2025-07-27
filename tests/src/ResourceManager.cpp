#include "TestDataDir.hpp"
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <array>
#include <catch2/catch_all.hpp>

class LoggableResource
{
public:
    static inline unsigned ctorCalledCount = 0;
    static inline unsigned moveCalledCount = 0;
    static inline unsigned dtorCalledCount = 0;

public:
    LoggableResource()
    {
        ctorCalledCount++;
    }

    LoggableResource(LoggableResource&&) noexcept
    {
        moveCalledCount++;
    }

    LoggableResource(const LoggableResource&) = delete;

    ~LoggableResource()
    {
        dtorCalledCount++;
    }
};

template<class T>
std::expected<T, dgm::Error> loadMocked(const std::filesystem::path&)
{
    return T {};
}

TEST_CASE("[ResourceManager]")
{
    dgm::ResourceManager resmgr;
    using path = std::filesystem::path;

    SECTION("Properly computes resource ID")
    {
        SECTION("From plain filename")
        {
            auto&& id = resmgr.getResourceId("file.ext");
            REQUIRE(id.has_value());
            REQUIRE(*id == "file.ext");

            auto&& id2 = resmgr.getResourceId("file_no_ext");
            REQUIRE(id2.has_value());
            REQUIRE(*id2 == "file_no_ext");
        }

        SECTION("From absolute path")
        {
            auto&& id = resmgr.getResourceId("C:/folder/file.ext");
            REQUIRE(id.has_value());
            REQUIRE(*id == "file.ext");
        }

        SECTION("From relative path")
        {
            auto&& id = resmgr.getResourceId("../folder/file.ext");
            REQUIRE(id.has_value());
            REQUIRE(*id == "file.ext");
        }

        SECTION("Throws on empty path")
        {
            REQUIRE(!resmgr.getResourceId("").has_value());
        }
    }

    SECTION("When loading resource")
    {
        SECTION("Resource ctor/dtor are called exactly once")
        {
            unsigned loadedCnt = LoggableResource::ctorCalledCount;
            unsigned movedCnt = LoggableResource::moveCalledCount;
            unsigned unloadedCnt = LoggableResource::dtorCalledCount;

            {
                dgm::ResourceManager resmgr2;
                REQUIRE(resmgr2.loadResource<LoggableResource>(
                    "path", loadMocked<LoggableResource>));
            }

            // resmgr2 is now destroyed, dtors should have been called
            // Regular constructor is called only once in load callback
            // The resource is then moved twice to destination memory
            // Destructor is called for each move and then for destroyed resmgr
            REQUIRE(loadedCnt + 1u == LoggableResource::ctorCalledCount);
            REQUIRE(movedCnt + 2u == LoggableResource::moveCalledCount);
            REQUIRE(unloadedCnt + 3u == LoggableResource::dtorCalledCount);
        }
    }

    SECTION("Can get reference to loaded resource")
    {
        REQUIRE(
            resmgr.loadResource<int>("myint", [](const path&) { return 42; }));
        REQUIRE(resmgr.get<int>("myint") == 42);
    }

    SECTION("Can list all resources of a given type")
    {
        REQUIRE(resmgr.loadResource<int>("a", loadMocked<int>));
        REQUIRE(resmgr.loadResource<int>("b", loadMocked<int>));
        REQUIRE(resmgr.loadResource<int>("c", loadMocked<int>));
        REQUIRE(resmgr.loadResource<double>("d", loadMocked<double>));

        const auto&& names = resmgr.getLoadedResourceIds<int>();
        REQUIRE(names.has_value());
        REQUIRE(names->size() == 3u);
        REQUIRE(names->at(0) == "a");
        REQUIRE(names->at(1) == "b");
        REQUIRE(names->at(2) == "c");

        const auto&& names2 = resmgr.getLoadedResourceIds<char>();
        REQUIRE(names2.has_value());
        REQUIRE(names2->empty());
    }

    SECTION("Cannot recursively load a directory")
    {
        dgm::JsonLoader loader;
        const auto&& dirPath = path { TEST_DATA_DIR } / "resmgr_loading";
        const auto&& loadClip = [](const path&)
        {
            return dgm::Clip(
                { 1, 1 },
                sf::IntRect {
                    sf::Vector2i(0, 0),
                    sf::Vector2i(1, 1),
                });
        };

        REQUIRE(resmgr.loadResourcesFromDirectory<dgm::Clip>(
            dirPath, loadClip, { ".json" }));

        REQUIRE(resmgr.hasResource<dgm::Clip>("statesA.json"));
        REQUIRE(!resmgr.hasResource<dgm::Clip>("nested.json"));
        REQUIRE(!resmgr.hasResource<dgm::Clip>("to_be_skipped.txt"));
    }

    SECTION("Can unload resource")
    {
        unsigned loadedCnt = LoggableResource::ctorCalledCount;
        unsigned movedCnt = LoggableResource::moveCalledCount;
        unsigned unloadedCnt = LoggableResource::dtorCalledCount;

        REQUIRE(resmgr.loadResource<LoggableResource>(
            "unloadable", loadMocked<LoggableResource>));
        std::ignore = resmgr.unloadResource<LoggableResource>("unloadable");

        REQUIRE(loadedCnt + 1u == LoggableResource::ctorCalledCount);
        REQUIRE(movedCnt + 2u == LoggableResource::moveCalledCount);
        REQUIRE(unloadedCnt + 3u == LoggableResource::dtorCalledCount);
    }

    SECTION("Can get mutable resource")
    {
        REQUIRE(resmgr.loadResource<int>("a", [](const path&) { return 69; }));
        REQUIRE(resmgr.get<int>("a") == 69);
        resmgr.getMutable<int>("a") = 42;
        REQUIRE(resmgr.get<int>("a") == 42);
    }

    SECTION("Does not crash when resource not in DB (throws instead)")
    {
        REQUIRE_THROWS_AS([&]() { resmgr.get<int>("none"); }(), dgm::Exception);
    }

    SECTION("Can insert resource")
    {
        auto&& image = sf::Image({ 256u, 256u }, sf::Color::White);
        auto&& texture = sf::Texture(image);

        SECTION("Via move")
        {
            auto result =
                resmgr.insertResource("my_texture", std::move(texture));
            REQUIRE(result);

            resmgr.unloadResource<sf::Texture>("my_texture");
        }
    }
}
