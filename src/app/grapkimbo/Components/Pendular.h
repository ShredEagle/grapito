#pragma once

#include <aunteater/Component.h>

#include <math/Angle.h>
#include <math/Vector.h>


namespace ad {


struct Pendular : public aunteater::Component<Pendular>
{
    Pendular(math::Position<2, double> aAnchor, math::Radian<double> aAngle, double aLength) :
        anchor{aAnchor},
        angle{aAngle},
        length{aLength}
    {}

    math::Position<2, double> anchor;
    math::Radian<double> angle;
    double length;
    math::Radian<double> angularSpeed{0.};
};



} // namespace ad
