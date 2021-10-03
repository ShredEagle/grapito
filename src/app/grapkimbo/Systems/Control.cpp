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
    mGrapplers{mEntityManager},
    mModeSelectables{mEntityManager},
    mAnchorables{mEntityManager}
{}

using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;


void Control::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    //
    // Air
    //
    for(auto & [controllable, geometry, aas, mass, playerData] :  mCartesianControllables)
    {
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];
        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);

        if (playerData.state == PlayerCollisionState_Grounded)
        {
            aas.speed += PlayerWalkingSpeed * horizontalAxis;

            if (inputs[Jump])
            {
                aas.speed += Vec2{0., + gJumpImpulse};
            }
        }
        else
        {

            aas.accel += Vec2{horizontalAxis * gAirControlAcceleration, 0.};

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

    //
    // Change grapple mode
    //
    for(const auto & entity : mModeSelectables)
    {
        auto & [controllable, grappleControl, playerData] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        if (inputs[ChangeMode].positiveEdge())
        {
            setGrappleMode(entity, playerData, toggle(grappleControl.mode), mEntityManager);
        }
    }
}


} // namespace grapito
} // namespace ad
