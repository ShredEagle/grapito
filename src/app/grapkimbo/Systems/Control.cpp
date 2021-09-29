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
            std::cout << "Grounded\n";
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
        auto & [controllable, geometry, pendular, weight] = entity;
        pendular.angularAccelerationControl = math::Radian<double>{0.};

        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        pendular.angularAccelerationControl = 
            math::Radian<double>{horizontalAxis * Gravity::gAcceleration / pendular.length 
                                 * gPendularControlAccelerationFactor};

        if (inputs[Jump])
        {
            retractGrapple(
                entity
            );
        }
    }

    //
    // Grapple candidates
    //
    for(const auto & entity : mGrapplers)
    {
        auto & [controllable, aas, grappleControl, geometry] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];


        switch (grappleControl.mode)
        {
        case GrappleMode::Closest:
            if (inputs[Grapple])
            {
                auto filter = [&](const AnchorWrap & anchor, Position2 aCandidate, Position2 aBasePosition, double aNormSquared)
                {
                    math::Vec<2, double> vec = aCandidate - aBasePosition;
                    return anchor->get<Body>().bodyType == BodyType_Static
                           // The candidate must be better than the previously selected candidate (i.e. closer to basePosition)
                           && vec.getNormSquared() < aNormSquared;
                };
                Position2 grappleOrigin = geometry.center();
                auto closest = getClosest(mAnchorables,
                                          grappleOrigin,
                                          [grappleOrigin](math::Rectangle<double> aRectangle)
                                          {
                                               return aRectangle.closestPoint(grappleOrigin);
                                          },
                                          filter);

                connectGrapple(entity, 
                               makePendular(grappleOrigin,
                                            closest->testedPosition,
                                            closest->entity,
                                            aas.speed,
                                            closest->distance));
            }
            break;

        case GrappleMode::AnchorSight:
            // Handled by ControlAnchorSight system
            break;
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
