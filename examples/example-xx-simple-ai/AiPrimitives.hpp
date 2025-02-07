/*
Functions in this files are primitive building blocks for the AI.

There are predicated that are used for picking transitions in FSM and then
there are logic chunks that can be used independently or chained together
using decorators.
*/

#pragma once

#include "Blackboard.hpp"

// PREDICATES
bool isCurrentNavpointReached(const Blackboard& board)
{
	auto mypos = board.myself.getBody().getPosition();
	auto navpoint = board.currentPath.getCurrentPoint();

	constexpr float THRESHOLD = 15.f;
	return dgm::Math::getSize(mypos - navpoint.coord) < THRESHOLD;
}

bool isPatrolPointReached(const Blackboard& board)
{
	return board.currentPath.isTraversed();
}

bool seesPlayer(const Blackboard& board)
{
	return dgm::Collision::basic(board.playerBody, board.visionCone);
}

// Logic chunks
void computePathToCurrentPatrolPoint(Blackboard& board)
{
	board.currentPath = board.navMesh.getPath(
		board.myself.getBody().getPosition(),
		board.patrolPoints[board.currentPatrolPointIndex]);
}

void goTowardsPoint(Blackboard& board, const sf::Vector2f& point)
{
	auto mypos = board.myself.getBody().getPosition();

	if (mypos.x > point.x)
		board.input.setInput(InputCode::Left);
	else if (mypos.x < point.x)
		board.input.setInput(InputCode::Right);
	if (mypos.y > point.y)
		board.input.setInput(InputCode::Up);
	else if (mypos.y < point.y)
		board.input.setInput(InputCode::Down);
}

void goTowardsNavpoint(Blackboard& board)
{
	goTowardsPoint(
		board,
		board.currentPath.getCurrentPoint().coord);
}

void goTowardsPlayer(Blackboard& board)
{
	goTowardsPoint(
		board,
		board.playerBody.getPosition());
}

void advanceNavpoint(Blackboard& board)
{
	board.currentPath.advance();
}

void pickNextPatrolPoint(Blackboard& board)
{
	board.currentPatrolPointIndex = (board.currentPatrolPointIndex + 1) % board.patrolPoints.size();
	// We could call computePath* here, but to keep this as primitive as possible
	// we can use dgm::fsm::decorator::Merge to do the same thing while building
}

void doNothing(Blackboard&)
{
	/* intentionally left blank */
}
