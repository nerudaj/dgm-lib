#include "TestDataDir.hpp"
#include <DGM/classes/JsonLoader.hpp>
#include <DGM/classes/ResourceManager.hpp>
#include <array>
#include <catch2/catch_all.hpp>

class LoggableResource
{
public:
    static inline unsigned ctorCalledCount = 0;
    static inline unsigned dtorCalledCount = 0;

public:
    LoggableResource()
    {
        ctorCalledCount++;
    }

    ~LoggableResource()
    {
        dtorCalledCount++;
    }
};

void loadLoggable(const std::filesystem::path&, LoggableResource&) {}

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
            {
                dgm::ResourceManager resmgr2;
                REQUIRE(resmgr2.loadResource<LoggableResource>(
                    "path", loadLoggable));
            }

            // resmgr2 is now destroyed, dtors should have been called
            REQUIRE(LoggableResource::ctorCalledCount == 1u);
            REQUIRE(LoggableResource::dtorCalledCount == 1u);
        }
    }

    SECTION("Can get reference to loaded resource")
    {
        REQUIRE(resmgr.loadResource<int>(
            "myint", [](const path&, int& val) { val = 42; }));
        auto&& val = resmgr.get<int>("myint");
        REQUIRE(val.has_value());
        REQUIRE(val->get() == 42);
    }

    SECTION("Can list all resources of a given type")
    {
        REQUIRE(resmgr.loadResource<int>("a", [](const path&, int&) {}));
        REQUIRE(resmgr.loadResource<int>("b", [](const path&, int&) {}));
        REQUIRE(resmgr.loadResource<int>("c", [](const path&, int&) {}));
        REQUIRE(resmgr.loadResource<double>("d", [](const path&, double&) {}));

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

    SECTION("Can recursively load a directory")
    {
        dgm::JsonLoader loader;
        const auto&& dirPath = path { TEST_DATA_DIR } / "resmgr_loading";
        const auto&& loadClip = [](const path&, dgm::Clip&)
        {
            /* not loading anything, those files are mocks */
        };

        REQUIRE(resmgr.loadResourcesFromDirectory<dgm::Clip>(
            dirPath, loadClip, { ".json" }));

        REQUIRE(resmgr.hasResource<dgm::Clip>("statesA.json"));
        REQUIRE(resmgr.hasResource<dgm::Clip>("statesB.json"));
        REQUIRE(!resmgr.hasResource<dgm::Clip>("to_be_skipped.txt"));
    }

    SECTION("Can unload resource")
    {
        unsigned loadedCnt = LoggableResource::ctorCalledCount;
        unsigned unloadedCnt = LoggableResource::dtorCalledCount;
        REQUIRE(
            resmgr.loadResource<LoggableResource>("unloadable", loadLoggable));
        resmgr.unloadResource<LoggableResource>("unloadable");

        REQUIRE(loadedCnt + 1u == LoggableResource::ctorCalledCount);
        REQUIRE(unloadedCnt + 1u == LoggableResource::dtorCalledCount);
    }
}
