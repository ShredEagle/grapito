#pragma once

#include "commons.h"

#include <aunteater/Component.h>

#include <graphics/commons.h>

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

typedef int ControlStateFlags;
typedef int ControlStateFlag;

constexpr ControlStateFlag ControlState_Throwing = 0b1;
constexpr ControlStateFlag ControlState_Attached = 0b10;

constexpr Vec2 PlayerGroundedNormal = {0.f, 1.f};
constexpr Vec2 PlayerWalledNormal = {1.f, 0.f};
constexpr float PlayerGroundedSlopeDotValue = 0.6f;
constexpr float PlayerWallSlopeDotValue = 0.8f;


struct PlayerData : public aunteater::Component<PlayerData>
{
    PlayerData(int aId, math::sdr::Rgb aColor) :
        id{aId},
        color{aColor}
    {}

    int id;
    math::sdr::Rgb color;
    PlayerCollisionState state = PlayerCollisionState_Jumping;
    ControlStateFlags controlState = 0;
    aunteater::weak_entity grapple;
    aunteater::weak_entity grappleAttachment;
};


} // namespace grapito
} // namespace ad
