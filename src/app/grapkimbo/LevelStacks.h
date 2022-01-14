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

void createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackTwo(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackFour(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackFive(aunteater::EntityManager & aEntityManager, float baseHeight);

}
}
