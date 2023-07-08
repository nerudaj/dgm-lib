#pragma once

#pragma warning(push, 0)
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#pragma warning(pop)

// DGM core
#include "classes/Animation.hpp"
#include "classes/App.hpp"
#include "classes/AppState.hpp"
#include "classes/Camera.hpp"
#include "classes/Clip.hpp"
#include "classes/Collision.hpp"
#include "classes/Controller.hpp"
#include "classes/Conversion.hpp"
#include "classes/DynamicBuffer.hpp"
#include "classes/Error.hpp"
#include "classes/JsonLoader.hpp"
#include "classes/LoaderInterface.hpp"
#include "classes/Math.hpp"
#include "classes/Objects.hpp"
#include "classes/ResourceManager.hpp"
#include "classes/SpatialBuffer.hpp"
#include "classes/StaticBuffer.hpp"
#include "classes/TileMap.hpp"
#include "classes/Time.hpp"
#include "classes/Window.hpp"

// Particle Systems
#include "classes/Particle.hpp"
#include "classes/ParticleSystem.hpp"
#include "classes/ParticleSystemRenderer.hpp"

// Navigation
#include "classes/NavMesh.hpp"
#include "classes/Path.hpp"

// Helpers
#include "classes/Traits.hpp"
#include "classes/Utility.hpp"
