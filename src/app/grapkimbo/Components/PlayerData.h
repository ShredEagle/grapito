#pragma once

#include "commons.h"

#include <aunteater/Component.h>

#include <graphics/commons.h>

namespace ad {
namespace grapito
{

typedef int PlayerCollisionStateFlags;
typedef int PlayerCollisionStateFlag;

constexpr PlayerCollisionStateFlag PlayerCollisionState_Grounded = 0b1;
constexpr PlayerCollisionStateFlag PlayerCollisionState_Walled = 0b10;
constexpr PlayerCollisionStateFlag PlayerCollisionState_WalledRight = 0b100;
constexpr PlayerCollisionStateFlag PlayerCollisionState_WalledLeft = 0b1000;
constexpr PlayerCollisionStateFlag PlayerCollisionState_Jumping = 0b10000;
constexpr PlayerCollisionStateFlag PlayerCollisionState_Grappling = 0b100000;
constexpr PlayerCollisionStateFlag PlayerCollisionState_OffGrapple = 0b1000000;

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
    PlayerCollisionStateFlags state = PlayerCollisionState_Jumping;
    ControlStateFlags controlState = 0;
    aunteater::weak_entity grapple;
    aunteater::weak_entity grappleAttachment;
    int wallClingFrameCounter = 0;
    aunteater::weak_entity mGrappleDistanceJoint = nullptr;
    aunteater::weak_entity mGrappleWeldJoint = nullptr;
    Vec2 mAimVector = Vec2::Zero();
};


} // namespace grapito
} // namespace ad
