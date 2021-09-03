#pragma once

#include "commons.h"
#include <aunteater/Component.h>

#include <math/Angle.h>
#include <math/Vector.h>


namespace ad {
namespace grapito
{


struct Pendular : public aunteater::Component<Pendular>
{
    Pendular(Position2 aAnchor,
             aunteater::weak_entity aConnected,
             math::Radian<double> aAngle,
             double aLength,
             math::Radian<double> aAngularSpeed = math::Radian<double>{0.}) :
        anchor{aAnchor},
        connected{aConnected},
        angle{aAngle},
        length{aLength},
        angularSpeed{aAngularSpeed}
    {}

    Position2 anchor;
    aunteater::weak_entity connected;
    math::Radian<double> angle;
    double length;
    math::Radian<double> angularSpeed;
    math::Radian<double> angularAccelerationControl{0.};
};



} // namespace grapito
} // namespace ad
