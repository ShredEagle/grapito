#pragma once

#include "Entities.h"
#include "aunteater/Entity.h"
#include "aunteater/EntityManager.h"
#include "commons.h"

namespace ad {
namespace grapito {

aunteater::Entity makeWall(Position2 aPos);

void createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackTwo(aunteater::EntityManager & aEntityManager, float baseHeight);
void createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight);

}
}
