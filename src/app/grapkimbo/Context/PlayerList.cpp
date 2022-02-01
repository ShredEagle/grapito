#include "PlayerList.h"

#include "Configuration.h"

#include <algorithm>

namespace ad {
namespace grapito {

int PlayerList::getNextSlot()
{
    std::array<bool, game::gMaxPlayerSlots - 1> availableSlots;
    std::fill(availableSlots.begin(), availableSlots.end(), false);
    for (auto player : mPlayerList)
    {
        availableSlots[player.mPlayerSlot] = true;
    }

    for (size_t i = 0; i < availableSlots.size(); i++)
    {
        if (!availableSlots[i])
        {
            return i;
        }
    }

    return -1;
}

PlayerJoinState PlayerList::getPlayerState(int aPlayerSlot)
{
    for (auto player : mPlayerList)
    {
        if (player.mPlayerSlot == aPlayerSlot)
        {
            return player.mJoinState;
        }
    }

    return PlayerJoinState_NotActive;
}

PlayerJoinState PlayerList::getPlayerState(Controller aControllerId)
{
    for (auto player : mPlayerList)
    {
        if (player.mControllerId == aControllerId)
        {
            return player.mJoinState;
        }
    }

    return PlayerJoinState_NotActive;
}

int PlayerList::addPlayer(Controller aControllerId, PlayerJoinState aJoinState)
{
    int availableSlot = getNextSlot();

    if (availableSlot != -1)
    {
        mPlayerList.emplace_back(PlayerControllerState{availableSlot, aControllerId, aJoinState});
    }

    return availableSlot;
}

void PlayerList::removePlayer(Controller aControllerId)
{
    for (auto playerIt = mPlayerList.begin(); playerIt != mPlayerList.end();)
    {
        PlayerControllerState player = *playerIt;
        if (player.mControllerId == aControllerId)
        {
            playerIt = mPlayerList.erase(playerIt);
        }
        else
        {
            ++playerIt;
        }
    }
}

std::vector<Controller> PlayerList::getUnactiveControllers()
{
    std::vector<Controller> unactiveControllers;
    
    for (auto controller : gAllControllers)
    {
        bool active = false;
        for (auto activePlayer : mPlayerList)
        {
            active = active || activePlayer.mControllerId == controller;
        }

        if (!active)
        {
            unactiveControllers.push_back(controller);
        }
    }

    return unactiveControllers;
}

}
}
