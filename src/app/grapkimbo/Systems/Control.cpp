#include "Control.h"

#include "Configuration.h"
#include "Gravity.h"
#include "../Components/SoundPlayer.h"
#include "../Components/DistanceJoint.h"

#include "../commons.h"
#include "../Entities.h"
#include "../Timer.h"
#include "../Utilities.h"

#include "../Utils/Player.h"
#include "math/Color.h"

#include <cmath>
#include <math/Constants.h>

#include <Components/VisualRectangle.h>

#include <handy/StringId_Interning.h>
#include <GLFW/glfw3.h>


namespace ad {
namespace grapito
{

const StringId soundId_JumpSid = handy::internalizeString("jump");
const StringId soundId_RopeJumpSid = handy::internalizeString("ropejump");

Control::Control(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCartesianControllables{mEntityManager},
    mGrapplers{mEntityManager}
{}

void Control::update(const GrapitoTimer, const GameInputState & aInputState)
{
    const float groundFriction = 1.f / player::gGroundNumberOfSlowFrame;
    const float airFriction = 1.f / player::gAirNumberOfSlowFrame;

    const float groundMaxFrameAcceleration = player::gGroundSpeed / player::gGroundNumberOfAccelFrame;
    const float airMaxFrameAcceleration = player::gAirSpeed / player::gAirNumberOfAccelFrame;

    //
    // General control
    //
    for(auto & player : mCartesianControllables)
    {
        auto & [controllable, geometry, aas, mass, playerData, body] = player;

        const ControllerInputState & inputs = aInputState.get(controllable.controller);
        float horizontalAxis = 
            aInputState.asAxis(controllable.controller, 
                               Left, Right, 
                               LeftHorizontalAxis, controller::gHorizontalDeadZone);
        Vec2 controllerDirection = 
            aInputState.asDirection(controllable.controller, 
                               LeftHorizontalAxis, LeftVerticalAxis, controller::gVerticalDeadZone, controller::gHorizontalDeadZone);

        //
        // Reset airborn jumps
        //
        if (playerData.state & PlayerCollisionState_Grounded || isAnchored(playerData))
        {
            // TODO Ad 2022/01/05: This should actually only be done once when the player transition to grounded.
            // (But currently this transition happens into the physics engine.)
            // Or when the player actually anchors to the environments.
            resetJumps(playerData);
        }

        //
        // Ground control to major Tom
        //
        if (playerData.state & PlayerCollisionState_Grounded)
        {
            if (std::abs(horizontalAxis) > 0.f)
            {
                if (std::abs(aas.speed.x()) <= player::gGroundSpeed)
                {
                    float targetSpeed = horizontalAxis * player::gGroundSpeed;
                    float speedChange = targetSpeed - aas.speed.x();
                    aas.speed += std::clamp(speedChange, -groundMaxFrameAcceleration, groundMaxFrameAcceleration) * Vec2{1.f, 0.f};
                }
                else
                {
                    // TODO FP Magic number
                    aas.accel -= aas.speed * 5.f;
                }
            }
            else
            {
                aas.speed *= (1.f - groundFriction);
            }

            if (inputs[Jump].positiveEdge())
            {
                addSoundToEntity(player, soundId_JumpSid);
                aas.speed.y() = player::gJumpImpulse;
            }
        }
        //
        // Air control
        //
        else if (playerData.state & PlayerCollisionState_Jumping)
        {
            if (std::abs(horizontalAxis) > 0.f && !isAnchored(playerData))
            {
                if (std::abs(aas.speed.x()) <= player::gAirSpeed)
                {
                    float targetSpeed = horizontalAxis * player::gAirSpeed;
                    float speedChange = targetSpeed - aas.speed.x();
                    aas.speed += std::clamp(speedChange, -airMaxFrameAcceleration, airMaxFrameAcceleration) * Vec2{1.f, 0.f};
                }
                else
                {
                    aas.accel.x() -= aas.speed.x();
                }
            }
            else if (controllerDirection.getNormSquared() > 0.f && isAnchored(playerData))
            {
                Position2 playerPos = body.massCenter + geometry.position.as<math::Vec>();
                DistanceJoint joint = playerData.mRopeContactDistanceJoint->get<DistanceJoint>();
                Position2 attachPoint = getLocalPointInWorld(joint.bodyB->get<Body>(), joint.bodyB->get<Position>(), joint.localAnchorB);

                Vec2 jointDirection = (attachPoint - playerPos).normalize();
                Vec2 possibleDirection = {-jointDirection.y(), jointDirection.x()};

                aas.speed += possibleDirection.dot(controllerDirection.normalize()) * 2.f * (player::gGrappleSwingSpeed / pow(possibleDirection.getNorm() + 1.f, 2)) * possibleDirection * controllerDirection.getNorm();

                debugDrawer->drawLine({playerPos, playerPos + possibleDirection * 4.f, 2.f, math::sdr::gGreen});
            }
            else
            {
                aas.speed.x() *= 1.f - airFriction;
            }

            //
            // Wall control
            //
            if (playerData.state & PlayerCollisionState_Walled && !isAnchored(playerData))
            {
                if (!inputs[Jump])
                {
                    aas.accel -= aas.speed * player::gWallFriction;
                }

                // Player wall succion
                // This is to avoid unstucking yourself from the wall to easily
                if (
                    playerData.wallClingFrameCounter < 10 &&
                    ((playerData.state & PlayerCollisionState_WalledLeft && aas.speed.x() > 0.f) ||
                    (playerData.state & PlayerCollisionState_WalledRight && aas.speed.x() < 0.f))
                )
                {
                    playerData.wallClingFrameCounter++;
                }

                if (playerData.wallClingFrameCounter < 10)
                {
                    aas.speed.x() = playerData.state & PlayerCollisionState_WalledLeft ? -1.f : 1.f;
                }
                else
                {
                    playerData.wallClingFrameCounter = 0;
                }

                if (
                        inputs[Jump].positiveEdge()
                        && !(playerData.state & PlayerCollisionState_Grounded)
                   )
                {
                    if (
                        ((horizontalAxis < 0.f && playerData.state & PlayerCollisionState_WalledLeft)
                        || (horizontalAxis > 0.f && playerData.state & PlayerCollisionState_WalledRight))
                        && playerData.airborneJumpsLeft > 0 && !isGrappleOut(playerData))
                    {
                        //
                        // Airborne jumps while walled
                        //
                        --playerData.airborneJumpsLeft;
                        aas.speed.y() = player::gJumpImpulse;
                    }
                    else
                    {
                        float wallJumpHorizontalImpulse = player::gJumpImpulse * player::gDoubleJumpFactor * cos(math::pi<float> / 4.);
                        float wallJumpVerticalImpulse = player::gJumpImpulse * player::gDoubleJumpFactor * sin(math::pi<float> / 4.);

                        aas.speed = playerData.state & PlayerCollisionState_WalledLeft ?
                            Vec2{wallJumpHorizontalImpulse, wallJumpVerticalImpulse} :
                            Vec2{-wallJumpHorizontalImpulse, wallJumpVerticalImpulse};

                        playerData.wallClingFrameCounter = 0;
                    }
                }
            }
            //
            // Airborne jumps
            //
            else if (inputs[Jump].positiveEdge() 
                     && playerData.airborneJumpsLeft > 0
                     && !isGrappleOut(playerData))
            {
                --playerData.airborneJumpsLeft;
                aas.speed.y() = player::gJumpImpulse;
            }
        }

        // Detach grapple from player if necessary
        if (isGrappleOut(playerData) && inputs[Jump].positiveEdge())
        {
            if (// The boosts are only granted if the player was successfully connected (no ABABAB speedrun).
                isAnchored(playerData) 
                // Otherwise, would make a super jump when both connected and grounded.
                && (playerData.state & PlayerCollisionState_Jumping))
            {
                aas.speed += aas.speed.normalize() * player::gGrappleReleaseImpulse;
                aas.speed.y() += player::gJumpImpulse;
            }

            detachPlayerFromGrapple(player);
        }
    }

    //
    // Grapple candidates
    //
    for(auto & player : mGrapplers)
    {
        auto & [controllable, aas, geometry, playerData] = player;
        const ControllerInputState & inputs = aInputState.get(controllable.controller);

        if (controllable.controller == Controller::KeyboardMouse)
        {
            playerData.mAimVector = {1.0f, 1.0f};
        }
        else
        {
            playerData.mAimVector = aInputState.asDirection(controllable.controller, LeftHorizontalAxis, LeftVerticalAxis, controller::gDeadzone);
            if (playerData.mAimVector.getNorm() < 0.1f)
            {
                playerData.mAimVector = aas.speed.getNorm() > 0.1f ? aas.speed / aas.speed.getNorm() : Vec2{0.f, 0.2f};
            }
            else
            {
                playerData.mAimVector.normalize();
            }
        }

        if (inputs[Grapple].positiveEdge() && !isGrappleOut(playerData))
        {
            throwGrapple(player, mEntityManager);
            playerData.controlState |= ControlState_Throwing;
        }

        if (inputs[Grapple].negativeEdge() && isThrowing(playerData))
        {
            attachPlayerToGrapple(player, mEntityManager);
            playerData.controlState &= ~ControlState_Throwing;
            playerData.controlState |= ControlState_Attached;
        }
    }


    for (auto& [controllable, geometry, aas, mass, playerData, body] : mCartesianControllables)
    {
        playerData.state &= ~PlayerCollisionState_Walled;
        playerData.state &= ~PlayerCollisionState_WalledLeft;
        playerData.state &= ~PlayerCollisionState_WalledRight;

        //Reset playerData transient state
        playerData.state |= PlayerCollisionState_Jumping;
        playerData.state &= ~PlayerCollisionState_Grounded;
    }
}


} // namespace grapito
} // namespace ad
