#pragma once


#include <math/Constants.h>
#include <math/Angle.h>
#include <math/Vector.h>


namespace ad {


namespace player
{
    constexpr double gMass = 80.;
    constexpr math::Size<2, double> gSize{0.8, 1.9};
    constexpr double gInitialRopeLength = 3.;
    constexpr math::Radian<double> gInitialAngle{math::pi<double>/3.};
}

namespace sight
{
    constexpr double gReachDistance = 20.  ;
    constexpr math::Degree<double> gAngleTolerance{40.};

    constexpr double gBaseOutlineOffset = 0.25;
} // namespace sight


} // namespace ad