#include "Control.h"

#include "Gravity.h"

#include "../Player.h"
#include "../Utilities.h"

#include <Components/VisualRectangle.h>

#include <GLFW/glfw3.h>


namespace ad {


Control::Control(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCartesianControllables{mEntityManager},
    mPolarControllables{mEntityManager},
    mGrapplers{mEntityManager},
    mModeSelectables{mEntityManager},
    mAnchorables{mEntityManager}
{}


void Control::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    //
    // Air
    //
    for(auto entity :  mCartesianControllables)
    {
        auto & [controllable, geometry, aas, weight] = entity;
        const ControllerInputState & inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        aas.accel += math::Vec<2, double>{horizontalAxis * gAirControlAcceleration, 0.};

        if (inputs[Jump])
        {
            aas.accel += math::Vec<2, double>{0., + gAirControlAcceleration};
            break;
        }
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
                math::Position<2, double> grappleOrigin = geometry.center();
                auto closest = getClosest(mAnchorables,
                                          grappleOrigin,
                                          [grappleOrigin](math::Rectangle<double> aRectangle)
                                          {
                                               return aRectangle.closestPoint(grappleOrigin);
                                          });

                connectGrapple(entity, 
                               makePendular(grappleOrigin,
                                            closest->testedPosition,
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


} // namespace ad
