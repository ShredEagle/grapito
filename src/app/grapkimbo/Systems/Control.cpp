#include "Control.h"

#include "Gravity.h"

#include <Components/VisualRectangle.h>

#include <GLFW/glfw3.h>


namespace ad {


Control::Control(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCartesianControllables{mEntityManager},
    mPolarControllables{mEntityManager},
    mAnchorables{mEntityManager}
{}


std::pair<math::Position<2, double>, double> Control::anchor(const math::Position<2, double> aPosition)
{
    math::Position<2, double> closest = math::Position<2, double>::Zero();
    double normSquared = std::numeric_limits<double>::max();

    for (const auto [geometry, _discard] : mAnchorables) 
    {
        math::Rectangle<double> box{geometry.position, geometry.dimension};
        math::Position<2, double> candidate = box.closestPoint(aPosition);
        if( (aPosition - candidate).getNormSquared() < normSquared)
        {
            normSquared = (aPosition - candidate).getNormSquared();
            closest = candidate;
        }
    }

    return {closest, std::sqrt(normSquared)};
}


template <class T_vecLeft, class T_vecRight>
math::Radian<double> angleBetween(T_vecLeft a, T_vecRight b)
{
    return math::Radian<double>{ std::atan2(b.y(), b.x()) - std::atan2(a.y(), a.x()) };
}


void Control::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for(auto entity :  mCartesianControllables)
    {
        auto & [controllable, geometry, fas, weight] = entity;
        ControllerInputState inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        fas.forces.emplace_back(horizontalAxis * gAirControlAcceleration * weight.mass, 0.);

        if (inputs[Grapple])
        {
            math::Position<2, double> grappleOrigin = geometry.position + (geometry.dimension / 2).as<math::Vec>();
            auto [anchorPoint, length] = this->anchor(grappleOrigin);
            math::Vec<2, double> grappleLine = anchorPoint - grappleOrigin;
            // grapple line goes from origin to anchor, we need the angle with -Y
            math::Radian<double> angle{std::atan2(-grappleLine.x(), grappleLine.y())};

            math::Vec<2, double> tangent{grappleLine.y(), - grappleLine.x()};
            math::Radian<double> angularSpeed{ cos(angleBetween(fas.speeds.at(0), tangent)) * fas.speeds.at(0).getNorm() / length };

            mEntityManager.markToRemove(entity);
            mEntityManager.addEntity(
               aunteater::Entity()
                .add<Position>(geometry)
                .add<VisualRectangle>(entity->get<VisualRectangle>())
                .add<Pendular>(Pendular{anchorPoint, angle, length, angularSpeed})
                .add<Controllable>(controllable)
                .add<Weight>(weight.mass)
            );
            break;
        }
        if (inputs[Jump])
        {
            fas.forces.emplace_back(0., + gAirControlAcceleration * weight.mass);
            break;
        }
    }

    //
    // Polar
    //
    for(auto & entity : mPolarControllables)
    {
        auto & [controllable, geometry, pendular, weight] = entity;
        pendular.angularAccelerationControl = math::Radian<double>{0.};

        ControllerInputState inputs = aInputState.controllerState[(std::size_t)controllable.controller];

        float horizontalAxis = aInputState.asAxis(controllable.controller, Left, Right, LeftHorizontalAxis);
        pendular.angularAccelerationControl = 
            math::Radian<double>{horizontalAxis * Gravity::gAcceleration / pendular.length 
                                 * gPendularControlAccelerationFactor};

        if (inputs[Jump])
        {
            // TODO if we edit the components on the live entity, everything crashes because the 
            // family are instantly edited to reflect the changes (invalidating iterators)
            // this shoud be reworked!
            mEntityManager.markToRemove(entity);
            mEntityManager.addEntity(
               aunteater::Entity()
                .add<Position>(geometry)
                .add<VisualRectangle>(entity->get<VisualRectangle>())
                .add<Controllable>(controllable)
                .add<Weight>(weight.mass)
                .add<ForceAndSpeed>(math::Vec<2>{
                    cos(pendular.angle) * pendular.length * pendular.angularSpeed.value(),
                    sin(pendular.angle) * pendular.length * pendular.angularSpeed.value()
                })
            );
        }
    }
}

} // namespace ad
