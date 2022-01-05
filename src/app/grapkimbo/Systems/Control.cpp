#include "Control.h"

#include "Configuration.h"
#include "Gravity.h"

#include "../commons.h"
#include "../Entities.h"
#include "../Utilities.h"

#include "../Utils/Player.h"

#include <math/Constants.h>

#include <Components/VisualRectangle.h>

#include <GLFW/glfw3.h>


namespace ad {
namespace grapito
{


Control::Control(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCartesianControllables{mEntityManager},
    mPolarControllables{mEntityManager},
    mGrapplers{mEntityManager}
{}

void Control::update(const GrapitoTimer, const GameInputState & aInputState)
{
    //
    // Air
    //
    for(auto & [controllable, geometry, aas, mass, playerData] :  mCartesianControllables)
    {
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];
        float horizontalAxis;

        if (controllable.controller == Controller::KeyboardMouse)
        {
            horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        }
        else
        {
            Vec2 direction = aInputState.asDirection(controllable.controller, LeftHorizontalAxis, LeftVerticalAxis, controller::gHorizontalDeadZone, 0.f);
            horizontalAxis = direction.x();
        }

        float groundFriction = 1.f / player::gGroundNumberOfSlowFrame;
        float airSpeedAccelFactor = 1.f / player::gAirNumberOfAccelFrame;
        float airFriction = 1.f / player::gAirNumberOfSlowFrame;

        const float groundMaxFrameAcceleration = player::gGroundSpeed / player::gGroundNumberOfAccelFrame;
        const float airMaxFrameAcceleration = player::gAirSpeed / player::gAirNumberOfAccelFrame;

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
                    aas.accel -= aas.speed * 5.f;
                }
            }
            else
            {
                aas.speed *= (1.f - groundFriction);
            }

            if (inputs[Jump].positiveEdge())
            {
                aas.speed.y() = player::gJumpImpulse;
            }
        }
        else if (playerData.state & PlayerCollisionState_Jumping)
        {
            if (std::abs(horizontalAxis) > 0.)
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
            else
            {
                aas.speed.x() *= 1.f - airFriction;
            }

            if (playerData.state & PlayerCollisionState_Walled)
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

                if (inputs[Jump].positiveEdge() && !(playerData.state & PlayerCollisionState_Grounded))
                {
                    float wallJumpHorizontalImpulse = player::gJumpImpulse * player::gDoubleJumpFactor * cos(math::pi<float> / 4.);
                    float wallJumpVerticalImpulse = player::gJumpImpulse * player::gDoubleJumpFactor * sin(math::pi<float> / 4.);

                    aas.speed = playerData.state & PlayerCollisionState_WalledLeft ?
                        Vec2{wallJumpHorizontalImpulse, wallJumpVerticalImpulse} :
                        Vec2{-wallJumpHorizontalImpulse, wallJumpVerticalImpulse};

                    playerData.wallClingFrameCounter = 0;
                }
            }
            else if (inputs[Jump].positiveEdge() 
                     && !isGrappleOut(playerData))
            {
                aas.speed.y() = player::gJumpImpulse;
            }
        }
    }

    //
    // Swinging on a grapple
    //
    for(auto & entity : mPolarControllables)
    {
        auto & [controllable, aas, playerData] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        if (inputs[Jump].positiveEdge() && isGrappleOut(playerData))
        {
            detachPlayerFromGrapple(entity);
            if (playerData.state & PlayerCollisionState_Jumping)
            {
                aas.speed *= 1.5f;
                aas.speed += Vec2{ 0.f, player::gJumpImpulse };
            }
            playerData.controlState &= ~ControlState_Attached;
            playerData.controlState &= ~ControlState_Throwing;
        }
    }

    //
    // Grapple candidates
    //
    for(auto & player : mGrapplers)
    {
        auto & [controllable, aas, grappleControl, geometry, playerData] = player;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

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


    for (auto& [controllable, geometry, aas, mass, playerData] : mCartesianControllables)
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
