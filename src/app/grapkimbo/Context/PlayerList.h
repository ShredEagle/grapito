#pragma once

#include "Input.h"

namespace ad {
namespace grapito {

enum PlayerJoinState
{
    PlayerJoinState_ChoosingColor, // Actually, this feels coupled to specific knowledge of the game.
                                   // Maybe this file should remain more abstract than that.
    PlayerJoinState_Queued, // Waiting to enter the next game
    PlayerJoinState_Playing,

    //Enum last member
    PlayerJoinState_NotActive,
};

struct PlayerControllerState
{
    int mPlayerSlot;
    Controller mControllerId;
    PlayerJoinState mJoinState;
    // TODO ability to pick a color
    math::sdr::Rgb mColor;
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
        /// \brief All controllers for which a player has not been associated (i.e. start has not been pressed).
        /// Whether the controller is connected to the system or not.
        std::vector<Controller> getUnactiveControllers();

        std::size_t countActivePlayers() const;

        auto begin()
        { return mPlayerList.begin(); }

        auto end()
        { return mPlayerList.end(); }

        auto begin() const
        { return mPlayerList.cbegin(); }

        auto end() const
        { return mPlayerList.cend(); }

    private:
        std::list<PlayerControllerState> mPlayerList;
};

} //grapito
} //ad
