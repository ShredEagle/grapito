#pragma once

#include <aunteater/Component.h>

#include <math/Angle.h>
#include <math/Vector.h>


namespace ad {


struct Pendular : public aunteater::Component<Pendular>
{
    Pendular(math::Position<2, double> aAnchor,
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

    math::Position<2, double> anchor;
    aunteater::weak_entity connected;
    math::Radian<double> angle;
    double length;
    math::Radian<double> angularSpeed;
    math::Radian<double> angularAccelerationControl{0.};
};



} // namespace ad
