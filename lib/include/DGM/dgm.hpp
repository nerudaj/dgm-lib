#pragma once

#pragma warning(push, 0)    
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <ciso646>	// keywords: and, or, and_eq, or_eq, xor, xor_eq, bitand, bitor, not, compl, not_eq

// dsh libraries    
#include <Config.hpp>

#include <Strings.hpp>
#include <LevelD.hpp>
#pragma warning(pop)

// DGM core
#include "classes/Error.hpp"
#include "classes/Buffer.hpp"
#include "classes/Objects.hpp"
#include "classes/Collision.hpp"
#include "classes/Math.hpp"
#include "classes/Conversion.hpp"
#include "classes/Controller.hpp"
#include "classes/Time.hpp"
#include "classes/Clip.hpp"
#include "classes/Window.hpp"
#include "classes/App.hpp"
#include "classes/AppState.hpp"
#include "classes/Animation.hpp"
#include "classes/TileMap.hpp"
#include "classes/ResourceManager.hpp"
#include "classes/Camera.hpp"

// Particle Systems
#include "classes/ParticleSystemRenderer.hpp"
#include "classes/Particle.hpp"
#include "classes/ParticleSystem.hpp"

// Navigation
#include "classes/Path.hpp"
#include "classes/NavMesh.hpp"

// Helpers
#include "classes/Traits.hpp"
#include "classes/Utility.hpp"
