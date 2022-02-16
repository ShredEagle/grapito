#pragma once


#include "Player.h"

#include "../Configuration.h"

#include "../Components/RopeCreator.h"


namespace ad {
namespace grapito {


inline bool isDetached(const RopeCreator & aRopeCreator)
{
    return aRopeCreator.mTargetEntity == nullptr;
}


inline bool isBeingThrown(const RopeCreator & aRopeCreator)
{
    return aRopeCreator.mTargetEntity != nullptr 
           && !isAnchored(aRopeCreator.mTargetEntity->get<PlayerData>());
}


// Note Ad 2022/01/11: 
// This is bad design, with some very implicit convention that
// a negative id is the player it was detached from.
inline int playerIdToDetachedId(int aPlayerId)
{
    // -1, otherwise 0 remains unchanged
    return -aPlayerId - 1;
}


inline int detachedIdToPlayerId(int aDetachedId)
{
    return -aDetachedId - 1;
}


} // namespace grapito
} // namespace ad
