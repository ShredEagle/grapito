#pragma once

#include "commons.h"
#include "engine/commons.h"
#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

enum PlayerCollisionState
{
    PlayerCollisionState_Grounded,
    PlayerCollisionState_Walled,
    PlayerCollisionState_Jumping,
    PlayerCollisionState_Grappling,
    PlayerCollisionState_OffGrapple,
};

constexpr Vec2 PlayerGroundedNormal = {0., 1.};
constexpr Vec2 PlayerWalledNormal = {1., 0.};
constexpr double PlayerGroundedSlopeDotValue = 0.6;
constexpr double PlayerWallSlopeDotValue = 0.8;
constexpr double PlayerGroundFriction = 0.25;

constexpr Vec2 PlayerWalkingSpeed = {10., 0.};


struct PlayerData : public aunteater::Component<PlayerData>
{
    PlayerData(int aId, Color aColor) :
        id{aId},
        color{aColor}
    {}

    int id;
    Color color;
    PlayerCollisionState state;
};


} // namespace grapito
} // namespace ad
