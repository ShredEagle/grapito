#include "GrappleCleanup.h"

#include "Components/PivotJoint.h"
#include "Components/PlayerData.h"
#include "Components/WeldJoint.h"
#include "Entities.h"
#include "../Timer.h"

#include "Components/Body.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "aunteater/Entity.h"
#include "aunteater/globals.h"
#include "math/Rectangle.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

GrappleCleanup::GrappleCleanup(aunteater::EntityManager & aEntityManager) :
    mCleanable{aEntityManager}
{}

void GrappleCleanup::update(const GrapitoTimer, const GameInputState &)
{
    for (auto & [playerData] : mCleanable)
    {
        if (playerData.mRopeContactDistanceJoint != nullptr)
        {
            playerData.mRopeContactDistanceJoint->markToRemove();
            playerData.mRopeContactDistanceJoint = nullptr;
        }

        if (playerData.mGrappleDistanceJoint != nullptr)
        {
            playerData.mGrappleDistanceJoint->markToRemove();
            playerData.mGrappleDistanceJoint = nullptr;
        }
    }
}


} // namespace grapito
} // namespace ad
