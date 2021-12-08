#pragma once


#include "../Configuration.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/PlayerData.h"


namespace ad {
namespace grapito {


/// \brief This file is working around the absence of a coherent "Player" class.

bool isIdle(const PlayerData & aPlayerData, const AccelAndSpeed & aAccelSpeed)
{
    return (aPlayerData.state & PlayerCollisionState_Grounded)
        && (std::abs(aAccelSpeed.speed.x()) < player::gIdleSpeedLimit);
}


} // namespace grapito
} // namespace ad
