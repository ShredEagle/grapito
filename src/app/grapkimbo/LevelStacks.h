#pragma once

#include "Entities.h"
#include "aunteater/Entity.h"
#include "aunteater/EntityManager.h"
#include "commons.h"

namespace ad {
namespace grapito {

extern const float gLevelHalfWidth;
extern const float gWallWidth;
extern const float gJumpHeight;
extern const float gStackHeight;

aunteater::Entity makeWall(Position2 aPos);
aunteater::Entity makeGrappleAnchor(Position2 aPos);

std::vector<aunteater::weak_entity> 
createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight);
std::vector<aunteater::weak_entity> 
createStackTwo(aunteater::EntityManager & aEntityManager, float baseHeight);
std::vector<aunteater::weak_entity> 
createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight);
std::vector<aunteater::weak_entity> 
createStackFour(aunteater::EntityManager & aEntityManager, float baseHeight);
std::vector<aunteater::weak_entity> 
createStackFive(aunteater::EntityManager & aEntityManager, float baseHeight);

}
}
