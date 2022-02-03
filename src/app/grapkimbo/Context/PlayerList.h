#pragma once

#include "Input.h"

namespace ad {
namespace grapito {

enum PlayerJoinState
{
    PlayerJoinState_ChoosingColor,
    PlayerJoinState_Playing,

    //Enum last member
    PlayerJoinState_NotActive,
};

struct PlayerControllerState
{
    int mPlayerSlot;
    Controller mControllerId;
    PlayerJoinState mJoinState;
};

class PlayerList
{
    public:
        PlayerList() = default;
        int getNextSlot();
        PlayerJoinState getPlayerState(int aPlayerSlot);
        PlayerJoinState getPlayerState(Controller aControllerId);
        int addPlayer(Controller aControllerId, PlayerJoinState aJointState);
        void removePlayer(Controller aControllerId);
        std::vector<Controller> getUnactiveControllers();
    private:
        std::list<PlayerControllerState> mPlayerList;
};

} //grapito
} //ad
