#pragma once

#include "../commons.h"
#include "../Configuration.h"

#include "../Animation/PlayerAnimation.h"

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
// TODO FP These seem not used?
constexpr PlayerCollisionStateFlag PlayerCollisionState_Grappling = 0b100000;
constexpr PlayerCollisionStateFlag PlayerCollisionState_OffGrapple = 0b1000000;

// TODO FP Why two different typedefs here?
typedef int ControlStateFlags;
typedef int ControlStateFlag;

constexpr ControlStateFlag ControlState_Throwing = 0b1;
// TODO FP The grapple is not necessarily attached though? (Might be out without connection)
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

    int wallClingFrameCounter = 0;
    int doubleJumpCooldownFrameCounter = 0;
    int throwGrappleFrameCounter = 0;
    int airborneJumpsLeft = player::gAirborneJumps;

    aunteater::weak_entity grapple = nullptr; // RopeCreator
    aunteater::weak_entity grappleAttachment = nullptr;
    aunteater::weak_entity mRopeContactDistanceJoint = nullptr;
    aunteater::weak_entity mGrappleWeldJoint = nullptr;
    aunteater::weak_entity mGrappleDistanceJoint = nullptr;

    Vec2 mAimVector = Vec2::Zero();

    // Note: Currently, the AnimatedSprite animationid is distinct from the player data current animation.
    // This way, this enum can be specialized for player animation states, while AnimatedSprite remains generic,
    // and several distinct PlayerAnimation states could use the same sprite animation.
    PlayerAnimation mCurrentAnimation{PlayerAnimation::Initial};
};


} // namespace grapito
} // namespace ad
