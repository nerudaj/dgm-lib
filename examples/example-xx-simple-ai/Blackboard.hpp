#pragma once

#include <DGM/dgm.hpp>
#include "Actor.hpp"
#include "NpcController.hpp"

/**
 *  Structure that holds all the data that could be need by AI primitives
 * 
 *  It is the only way how you can pass parameters down to those functions
 */
struct Blackboard
{
    Actor& myself;
    NpcController& input;
    dgm::VisionCone& visionCone;
    dgm::WorldNavMesh& navMesh;
    unsigned currentPatrolPointIndex = 0;
    std::vector<sf::Vector2f> patrolPoints = {
        { 130.f, 130.f },
        { 220.f, 670.f },
        { 640.f, 610.f },
        { 825.f, 120.f },
    };
    dgm::Path<dgm::WorldNavpoint> currentPath;
    dgm::Circle playerBody;
};
