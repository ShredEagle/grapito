#pragma once


#include "../Configuration.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/PlayerData.h"


namespace ad {
namespace grapito {


/// \brief This file is working around the absence of a coherent "Player" class.


inline bool isIdle(const PlayerData & aPlayerData, const AccelAndSpeed & aAccelSpeed)
{
    return (aPlayerData.state & PlayerCollisionState_Grounded)
        && (std::abs(aAccelSpeed.speed.x()) < player::gIdleSpeedLimit);
}


inline bool isGoingLeft(const AccelAndSpeed & aAccelSpeed)
{
    return aAccelSpeed.speed.x() < -player::gIdleSpeedLimit;
}


inline bool isGoingRight(const AccelAndSpeed & aAccelSpeed)
{
    return aAccelSpeed.speed.x() > player::gIdleSpeedLimit;
}


inline bool isGrounded(const PlayerData & aPlayerData)
{
    return aPlayerData.state & PlayerCollisionState_Grounded;
}


inline bool isJumping(const PlayerData & aPlayerData)
{
    return aPlayerData.state & PlayerCollisionState_Jumping;
}


inline bool isWalledLeft(const PlayerData & aPlayerData)
{
    return aPlayerData.state & PlayerCollisionState_WalledLeft;
}


inline bool isWalledRight(const PlayerData & aPlayerData)
{
    return aPlayerData.state & PlayerCollisionState_WalledRight;
}


inline bool isGrappleOut(const PlayerData & aPlayerData)
{
    return aPlayerData.controlState & (ControlState_Attached | ControlState_Throwing);
}


inline bool isThrowing(const PlayerData & aPlayerData)
{
    return aPlayerData.controlState & ControlState_Throwing;
}


inline bool isAnchored(const PlayerData & aPlayerData)
{
    return aPlayerData.mGrappleWeldJoint != nullptr || aPlayerData.mRopeContactDistanceJoint != nullptr;
}


inline void resetJumps(PlayerData & aPlayerData)
{
    aPlayerData.airborneJumpsLeft = player::gAirborneJumps;
}


} // namespace grapito
} // namespace ad
