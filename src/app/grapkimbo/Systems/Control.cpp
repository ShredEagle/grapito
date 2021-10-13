#include "Control.h"

#include "Gravity.h"

#include "../Entities.h"
#include "../Utilities.h"
#include "commons.h"

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
        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        float horizontalAxisSign = horizontalAxis / std::abs(horizontalAxis);

        if (playerData.state == PlayerCollisionState_Grounded)
        {
            if (aas.speed.getNorm() < player::gPlayerWalkingSpeed && std::abs(horizontalAxis) > 0.)
            {
                aas.speed += horizontalAxisSign * (1.f / (1.f - player::gPlayerGroundFriction)) * player::gPlayerWalkingSpeed * player::gWalkingSpeedAccelFactor * Vec2{1.f, 0.f};
            }

            if (inputs[Jump])
            {
                aas.speed += Vec2{0.f, + player::gPlayerJumpImpulse};
            }
        }
        else
        {
            if (std::abs(aas.speed.x()) < player::gPlayerWalkingSpeed && std::abs(horizontalAxis) > 0.)
            {
                aas.speed += horizontalAxisSign * (1.f / (1.f - player::gAirFriction)) * player::gAirControlAcceleration * player::gAirSpeedAccelFactor * Vec2{1.f, 0.f};
            }
            aas.speed.x() *= (1.f - player::gAirFriction);
        }

        playerData.state = PlayerCollisionState_Jumping;
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
