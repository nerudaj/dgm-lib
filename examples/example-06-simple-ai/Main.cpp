/**
 *  This example shows how you can build an AI with the help of dgm-fsm-lib and native dgm-lib primitives
 *  like vision cone for visibility checks.
 *
 *  Blue circle is an AI on a patrol route and yellow circle is you. You can move by using WASD.
 *  If you step into vision cone of an AI, it will start hunting you, when it loses you, it'll
 *  return to its patrol route.
 *
 *  The vision detection isn't perfect, so it is not hard to lose the AI, but that
 *  is besides the point of this example - the point is how to you can put together
 *  a FSM using dgm-fsm-lib.
 */

#include "../shared/Level.hpp"
#include "../shared/DemoData.hpp"
#include "../ResourceDir.hpp"
#include <DGM/dgm.hpp>

#include "Scene.hpp"
#include "Actor.hpp"
#include "PhysicalController.hpp"
#include "NpcController.hpp"
#include "NpcBrain.hpp"

int main(int argc, char* argv[])
{
	dgm::Window window({ 1280, 980 }, "Example: Simple AI", false);
	dgm::Time time;

	dgm::ResourceManager resmgr;
	resmgr.loadResourcesFromDirectory<sf::Texture>(
		RESOURCE_DIR,
		[](const std::filesystem::path& path, sf::Texture& texture)
		{
			texture.loadFromFile(path.string());
		},
		{ ".png" });

	Level level(resmgr.get<sf::Texture>("tileset.png").value().get());
	level.loadFromLvd(DemoData::createDemoLevel());

	PhysicalController playerInput;
	Actor player(128.f, sf::Color::Yellow, playerInput);
	player.spawn({ 96.f, 96.f });

	NpcController npcInput;
	Actor npc(110.f, sf::Color::Blue, npcInput);
	npc.spawn({ 577.f, 379.f });
	NpcBrain npcBrain(npc, npcInput, level.getMesh());

	Scene scene = {
		.level = level,
		.player = player,
		.npc = npc
	};

	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		/* LOGIC */
		time.reset();

		player.update(time, scene);
		npcBrain.update(scene);
		npc.update(time, scene);

		/* DRAW */
		window.beginDraw();

		level.draw(window);
		npcBrain.drawVisionConeTo(window);
		npc.drawTo(window);
		player.drawTo(window);

		window.endDraw();
	}

	return 0;
}