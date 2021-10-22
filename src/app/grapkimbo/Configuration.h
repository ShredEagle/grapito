#pragma once

#include <math/Constants.h>
#include <math/Angle.h>
#include <math/Color.h>
#include <math/Vector.h>

#include <glad/glad.h>

namespace ad {
namespace anchor
{
    constexpr math::sdr::Rgb gColor{200, 200, 200};
} // namespace anchor


namespace camera
{
    constexpr float gAnchorGuideFadeIn = 2.f;
    constexpr float gAnchorGuideFadeOut = 0.8f;
}


namespace debug
{
    constexpr float gStepTimeIncrement = 0.016f;
};


namespace player
{
    constexpr float gMass = 80.f;
    constexpr math::Size<2, float> gSize{0.8f, 1.9f};
    constexpr float gInitialRopeLength = 3.f;
    constexpr math::Radian<float> gInitialAngle{math::pi<float>/3.f};
    //TODO(franz) replace with constexpr
    inline float gWalkingSpeedAccelFactor = 0.25f;
    inline float gAcceleration = 70.f;
    inline float gPlayerWalkingSpeed = 12.f;
    inline float gPlayerGroundFriction = 0.3f;
    inline float gAirSpeedAccelFactor = 0.25f;
    inline float gPlayerJumpImpulse = 24.f; // m/s
    inline float gAirControlAcceleration = 12.f; // m/s
    inline float gAirFriction = 0.1f; // m/s
    //static constexpr double gAirControlAcceleration = 12.; // m/s
    //constexpr double gAcceleration = 10.;
    //constexpr double gPlayerJumpImpulse = 10.; // m/s
    //constexpr double gPlayerWalkingSpeed = 7.;
    //constexpr double gPlayerGroundFriction = 0.25;

} // namespace player


namespace render
{
    constexpr float gViewedHeight = 50.f;
    constexpr GLsizei gBezierSubdivisions = 30;
} // namespace render


namespace game
{
    constexpr math::hdr::Rgb gClearColor{0.1, 0.2, 0.3};
} // namespace game


namespace splash
{
    constexpr int gVerticalResolution = 800;
    constexpr float gDuration = 3.5f;
} // namespace splash

namespace menu
{
    constexpr float gViewedHeight = 1000.f;
    constexpr math::Size<2, GLfloat> gButtonSize{320.f, 80.f};
    constexpr float gButtonSpacing = 100.f;
    constexpr math::sdr::Rgb gButtonColor{ 120, 100, 110 };
    constexpr math::sdr::Rgb gSelectedColor{ 190, 190, 80 };
}

namespace sight
{
    constexpr float gReachDistance = 20.f;
    constexpr math::Degree<float> gAngleTolerance{40.f};

    constexpr float gBaseOutlineOffset = 0.25f;
} // namespace sight

namespace physic
{
    constexpr float linearSlop = 0.0001f;
}

namespace rope
{
    constexpr GLfloat ropeWidth = .2f;
    constexpr GLfloat ropeHalfwidth = ropeWidth / 2 ;
    constexpr GLfloat curveTension = 0.f; // I.e. Catmull Rom
}


} // namespace ad
