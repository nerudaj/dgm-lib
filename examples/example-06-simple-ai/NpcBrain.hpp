#pragma once

#include <DGM/dgm.hpp>
#include "Actor.hpp"
#include "Scene.hpp"
#include "Blackboard.hpp"
#include <DGM/fsm.hpp>

class NpcBrain
{
protected:
	enum class State
	{
		Start = 0,
		Patrolling = 1,
		UpdateNavpoint = 2,
		AfterNavpointUpdate = 3,
		PickNextPatrolPoint = 4,
		ChasingPlayer = 5,
	};

protected:
	Actor& puppet;
	NpcController& puppetInput;
	dgm::fsm::Fsm<Blackboard, State> fsm;
	dgm::VisionCone visionCone;
	dgm::WorldNavMesh navMesh;
	std::vector<sf::Vector2f> patrolPoints;
	Blackboard blackboard;

protected:
	void initFsm();

public:
	NpcBrain(Actor& puppet, NpcController& puppetInput, const dgm::Mesh& levelMesh);

	void update(const Scene& scene);

	void drawVisionConeTo(dgm::Window& window)
	{
		visionCone.debugRender(window, fsm.getState() == State::ChasingPlayer ? sf::Color::Cyan : sf::Color::Magenta);
	}
};
