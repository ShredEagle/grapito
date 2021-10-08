#pragma once


#include <math/Constants.h>
#include <math/Angle.h>
#include <math/Color.h>
#include <math/Vector.h>


namespace ad {


namespace anchor
{
    constexpr math::sdr::Rgb gColor{200, 200, 200};
} // namespace anchor


namespace camera
{
    constexpr double gAnchorGuideFadeIn = 2.;
    constexpr double gAnchorGuideFadeOut = 0.8;
}


namespace player
{
    constexpr double gMass = 80.;
    constexpr math::Size<2, double> gSize{0.8, 1.9};
    constexpr double gInitialRopeLength = 3.;
    constexpr math::Radian<double> gInitialAngle{math::pi<double>/3.};
} // namespace player


namespace render
{
    constexpr float gViewedHeight = 50.f;
    constexpr GLsizei gBezierSubdivisions = 30;
} // namespace render


namespace sight
{
    constexpr double gReachDistance = 20.  ;
    constexpr math::Degree<double> gAngleTolerance{40.};

    constexpr double gBaseOutlineOffset = 0.25;
} // namespace sight

namespace physic
{
    constexpr double linearSlop = 0.0001;
}

namespace rope
{
    constexpr GLfloat ropeWidth = .2f;
    constexpr GLfloat ropeHalfwidth = ropeWidth / 2 ;
    constexpr GLfloat curveTension = 0.f; // I.e. Catmull Rom
}


} // namespace ad
