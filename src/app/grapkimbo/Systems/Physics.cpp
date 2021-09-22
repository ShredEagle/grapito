#include "Physics.h"

namespace ad {
namespace grapito
{

Physics::Physics(aunteater::EntityManager & aEntityManager)
{
    aEntityManager.getFamily<PhysicalBody>().registerObserver
}

void Physics::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    // Broad phase
    //
    // We want this to be short and sweet to quickly get out of the list
    // structure
    // an improvement would be to use a tree
    for (auto bodyAIt = constructedBodies.begin(); bodyAIt != --constructedBodies.end(); ++bodyAIt)
    {
        auto bodyA = *bodyAIt;

        auto bodyBIt = bodyAIt;
        bodyBIt++;

        for (; bodyBIt != constructedBodies.end(); ++bodyBIt)
        {
            auto bodyB = *bodyBIt;

            auto aabbA = bodyA.box.shape.getAABB();
            auto aabbB = bodyB.box.shape.getAABB();

            //Basic SAT for AABB
            if (aabbA.x() < aabbB.x() + aabbB.width() &&
               aabbA.x() + aabbA.width() > aabbB.x() &&
               aabbA.y() < aabbB.y() + aabbB.height() &&
               aabbA.y() + aabbA.height() > aabbB.y() &&
            {
                collidingBodies.push_back({bodyA, bodyB});
            }
        }
    }

        //Collide pairs
        

        //Solve constraints
    }
}

} // namespace grapito
} // namespace ad
