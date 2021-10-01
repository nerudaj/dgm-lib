#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#ifndef NO_NETWORK
	#include <SFML/Network.hpp>
#endif

#ifndef _DEBUG
	#pragma comment(lib, "sfml-main.lib")
	#pragma comment(lib, "sfml-system.lib")
	#pragma comment(lib, "sfml-graphics.lib")
	#pragma comment(lib, "sfml-window.lib")
	#pragma comment(lib, "sfml-audio.lib")

#ifndef NO_NETWORK
	#pragma comment(lib, "sfml-network.lib")
#endif
#else
	#pragma comment(lib, "sfml-main-d.lib")
	#pragma comment(lib, "sfml-system-d.lib")
	#pragma comment(lib, "sfml-graphics-d.lib")
	#pragma comment(lib, "sfml-window-d.lib")
	#pragma comment(lib, "sfml-audio-d.lib")

#ifndef NO_NETWORK
	#pragma comment(lib, "sfml-network-d.lib")
#endif
#endif

#include <ciso646>	// keywords: and, or, and_eq, or_eq, xor, xor_eq, bitand, bitor, not, compl, not_eq

// dsh libraries
#include <Config.hpp>
#include <Strings.hpp>
#include <LevelD.hpp>

// DGM core
#include "classes/Error.hpp"
#include "classes/Buffer.hpp"
#include "classes/RingBuffer.hpp"
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
#include "classes/Shell.hpp"
#include "classes/Camera.hpp"

// Particle Systems
#include "classes/ParticleSystemRenderer.hpp"
#include "classes/Particle.hpp"
#include "classes/ParticleSystem.hpp"