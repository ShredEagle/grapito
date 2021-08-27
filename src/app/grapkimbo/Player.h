#pragma once


#include "Components/Pendular.h"

#include <math/VectorUtilities.h>


namespace ad {


inline void connectGrapple(aunteater::weak_entity aEntity, Pendular aPendular)
{
    (*aEntity)
        .markComponentToRemove<ForceAndSpeed>()
        .add<Pendular>(std::move(aPendular))
        ;
}


inline void retractGrapple(aunteater::weak_entity aEntity, ForceAndSpeed aForceAndSpeed)
{
    (*aEntity)
        .markComponentToRemove<Pendular>()
        .add<ForceAndSpeed>(std::move(aForceAndSpeed))
        ;
}


inline Pendular makePendular(math::Position<2, double> aGrappleOrigin, math::Position<2, double> aAnchorPoint, math::Vec<2, double> aCartesianSpeed, double aGrappleLength)
{
    math::Vec<2, double> grappleLine = aAnchorPoint - aGrappleOrigin;
    // grapple line goes from origin to anchor, we need the angle with -Y
    math::Radian<double> angle{std::atan2(-grappleLine.x(), grappleLine.y())};

    math::Vec<2, double> tangent{grappleLine.y(), - grappleLine.x()};
    math::Radian<double> angularSpeed{ cos(math::getOrientedAngle(aCartesianSpeed, tangent)) 
                                       * aCartesianSpeed.getNorm() / aGrappleLength };

    return Pendular{aAnchorPoint, angle, aGrappleLength, angularSpeed};
}


inline Pendular makePendular(math::Position<2, double> aGrappleOrigin, math::Position<2, double> aAnchorPoint, math::Vec<2, double> aCartesianSpeed)
{
    return makePendular(aGrappleOrigin, aAnchorPoint, aCartesianSpeed, (aAnchorPoint - aGrappleOrigin).getNorm());
}

} // namespace ad
