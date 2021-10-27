#include "Control.h"

#include "Configuration.h"
#include "Gravity.h"

#include "../Entities.h"
#include "../Utilities.h"
#include "commons.h"
#include "math/Constants.h"

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

void Control::update(const GrapitoTimer aTimer, const GameInputState & aInputState)
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
            Vec2 direction = aInputState.asDirection(controllable.controller, LeftHorizontalAxis, LeftVerticalAxis, controller::gDeadzone);
            horizontalAxis = direction.x();
        }

        float horizontalAxisSign = horizontalAxis / std::abs(horizontalAxis);

        float groundSpeedAccelFactor = 1.f / player::gGroundNumberOfAccelFrame;
        float groundFriction = 1.f / player::gGroundNumberOfSlowFrame;
        float airSpeedAccelFactor = 1.f / player::gAirNumberOfAccelFrame;
        float airFriction = 1.f / player::gAirNumberOfSlowFrame;

        if (playerData.state & PlayerCollisionState_Grounded)
        {
            if (std::abs(horizontalAxis) > 0.f)
            {
                if (std::abs(aas.speed.x()) <= player::gGroundSpeed)
                {
                    aas.speed += horizontalAxisSign * player::gGroundSpeed * groundSpeedAccelFactor * Vec2{1.f, 0.f};
                    aas.speed.x() = std::max(std::min(player::gGroundSpeed, aas.speed.x()), -player::gGroundSpeed);
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
                aas.speed += Vec2{0.f, + player::gJumpImpulse};
            }
        }
        else if (playerData.state & PlayerCollisionState_Jumping)
        {
            if (std::abs(horizontalAxis) > 0.)
            {
                if (std::abs(aas.speed.x()) <= player::gGroundSpeed)
                {
                    aas.speed += horizontalAxisSign * player::gAirSpeed * airSpeedAccelFactor * Vec2{1.f, 0.f};
                    aas.speed.x() = std::max(std::min(player::gAirSpeed, aas.speed.x()), -player::gAirSpeed);
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
        }

        playerData.state &= ~PlayerCollisionState_Walled;
        playerData.state &= ~PlayerCollisionState_WalledLeft;
        playerData.state &= ~PlayerCollisionState_WalledRight;

        //Reset playerData transient state
        playerData.state |= PlayerCollisionState_Jumping;
        playerData.state &= ~PlayerCollisionState_Grounded;
    }

    //
    // Swinging on a grapple
    //
    for(auto & entity : mPolarControllables)
    {
        auto & [controllable, playerData] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        if (inputs[Jump].positiveEdge() && playerData.controlState & ControlState_Attached)
        {
            detachPlayerFromGrapple(entity);
            playerData.controlState &= ~ControlState_Attached;
        }
    }

    //
    // Grapple candidates
    //
    for(auto & entity : mGrapplers)
    {
        auto & [controllable, aas, grappleControl, geometry, playerData] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        if (controllable.controller == Controller::KeyboardMouse)
        {
            playerData.mAimVector = {1.0f, 1.0f};
        }
        else
        {
            playerData.mAimVector = aInputState.asDirection(controllable.controller, RightHorizontalAxis, RightVerticalAxis, 0.25f);
        }

        if (inputs[Grapple].positiveEdge() && !(playerData.controlState & ControlState_Attached))
        {
            throwGrapple(entity, mEntityManager);
            playerData.controlState |= ControlState_Throwing;
        }
        if (inputs[Grapple].negativeEdge() && playerData.controlState & ControlState_Throwing)
        {
            attachPlayerToGrapple(entity, mEntityManager);
            playerData.controlState &= ~ControlState_Throwing;
            playerData.controlState |= ControlState_Attached;
        }

    }
}


} // namespace grapito
} // namespace ad
