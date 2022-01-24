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


} // namespace grapito
} // namespace ad
