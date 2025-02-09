#include "NpcBrain.hpp"
#include "AiPrimitives.hpp"

NpcBrain::NpcBrain(Actor& puppet, NpcController& puppetInput, const dgm::Mesh& levelMesh)
	: puppet(puppet)
	, puppetInput(puppetInput)
	, navMesh(dgm::WorldNavMesh(levelMesh))
	, visionCone(dgm::VisionCone(192.f, 256.f))
	, blackboard({
			puppet,
			puppetInput,
			visionCone,
			navMesh,
		})
{
	initFsm();
}

void NpcBrain::initFsm()
{
	using namespace dgm::fsm::decorator;

	// This bit constructs the final state machine and links together
	// ai primitives to some actual logic
	fsm = dgm::fsm::Builder<Blackboard, State>()
		.with(State::Start)
		.exec(computePathToCurrentPatrolPoint).andGoTo(State::Patrolling)

		.with(State::Patrolling)
		.when(isCurrentNavpointReached).goTo(State::UpdateNavpoint)
		.orWhen(seesPlayer).goTo(State::ChasingPlayer)
		.otherwiseExec(goTowardsNavpoint).andLoop()

		.with(State::UpdateNavpoint)
		.exec(advanceNavpoint).andGoTo(State::AfterNavpointUpdate)

		.with(State::AfterNavpointUpdate)
		.when(isPatrolPointReached).goTo(State::PickNextPatrolPoint)
		.otherwiseExec(doNothing).andGoTo(State::Patrolling)

		// template parameter in Merge is mandatory,
		// cause C++ type deduction sucks
		.with(State::PickNextPatrolPoint)
		.exec(Merge<Blackboard>(
			pickNextPatrolPoint,
			computePathToCurrentPatrolPoint
		)).andGoTo(State::Patrolling)

		.with(State::ChasingPlayer)
		.when(Not<Blackboard>(seesPlayer)).goTo(State::Start)
		.otherwiseExec(goTowardsPlayer).andLoop()

		.build();
}

void NpcBrain::update(const Scene& scene)
{
	puppetInput.releaseAllInputs();

	visionCone.setPosition(puppet.getBody().getPosition());
	visionCone.setRotation(dgm::Math::cartesianToPolar(puppet.getLookDir()).angle);

	blackboard.playerBody = scene.player.getBody();
	fsm.update(blackboard);
}
