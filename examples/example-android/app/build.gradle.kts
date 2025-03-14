val NDK_VERSION by extra(project.properties["NDK_VERSION"] as? String ?: "26.1.10909125")
val ARCH_ABI by extra(project.properties["ARCH_ABI"] as? String ?: "arm64-v8a")
val MIN_SDK by extra((project.properties["MIN_SDK"] as? String ?: "21").toInt())
val TARGET_SDK by extra((project.properties["TARGET_SDK"] as? String ?: "33").toInt())

plugins {
    id("com.android.application")
}

android {
    namespace = "org.nerudaj.example"
    ndkVersion = NDK_VERSION
    compileSdk = TARGET_SDK
	
	sourceSets {
        getByName("main") {
            assets.srcDirs("../assets")
        }
	}
	
    defaultConfig {
        applicationId = "org.nerudaj.example"
        minSdk = MIN_SDK
        targetSdk = TARGET_SDK
        versionCode = 1
        versionName = "1.0"
        ndk {
            abiFilters.add(ARCH_ABI)
        }
        externalNativeBuild {
            cmake {
                arguments.add("-DANDROID_STL=c++_shared")
                arguments.add("-DSFML_STATIC_LIBRARIES=OFF")
            }
        }
    }
	
    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android.txt"), "proguard-rules.pro")
        }
    }
    externalNativeBuild {
        cmake {
            path("src/main/cpp/CMakeLists.txt")
			version = "3.28.1"
        }
    }
}

dependencies {
    implementation(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
}
