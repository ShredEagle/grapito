#pragma once

#include "commons.h"
#include <math/Constants.h>
#include <math/Angle.h>
#include <math/Color.h>
#include <math/Vector.h>

#include <glad/glad.h>
#include <vector>

namespace ad {
namespace grapito {


namespace anchor
{
    constexpr math::sdr::Rgb gColor{200, 200, 200};
} // namespace anchor


namespace camera
{
    constexpr float gAnchorGuideFadeIn = 2.f;
    constexpr float gAnchorGuideFadeOut = 0.8f;
}


namespace controller
{
    constexpr float gDeadzone = 0.8f;
    constexpr float gHorizontalDeadZone = 0.4f;
    constexpr float gVerticalDeadZone = 0.2f;
}


namespace debug
{
    constexpr float gStepTimeIncrement = 0.016f;
};


namespace game
{
    extern const math::Size<2, int> gAppResolution;
    constexpr math::hdr::Rgb gClearColor{0.1, 0.2, 0.3};
} // namespace game


namespace menu
{
    constexpr int gBlurringPasses = 6;
    constexpr float gViewedHeight = 1000.f;
    constexpr math::Size<2, GLfloat> gButtonSize{360.f, 80.f};
    constexpr char * gFont = "fonts/dejavu-fonts-ttf-2.37/ttf/DejaVuSans.ttf";
    constexpr GLfloat gTextHeight = 0.65f * gButtonSize.height();
    constexpr float gButtonSpacing = 100.f;
    constexpr math::sdr::Rgb gButtonColor{ 120, 100, 110 };
    constexpr math::sdr::Rgb gSelectedColor{ 190, 190, 80 };
    constexpr float gTransitionDuration = 0.275f;
}


namespace physic
{
    //This is the error that is allowed during physics resolution
    //This represent the separation that we expect at rest
    constexpr float gLinearSlop = 0.0001f;
    constexpr float gAngularSlop = 2.f / 180 * math::pi<float>;
    constexpr float gBaumgarteFactor = 0.2f;
    constexpr float gMaxLinearCorrection = 0.2f;
    constexpr int gMaxVelocityConstraintIteration = 50;
    constexpr int gMaxPositionConstraintIteration = 50;
}


namespace player
{
    constexpr float gMass = 80.f;
    constexpr math::Size<2, float> gSize{0.8f, 1.9f};
    constexpr float gInitialRopeLength = 3.f;
    constexpr math::Radian<float> gInitialAngle{math::pi<float>/3.f};
    //TODO(franz) replace with constexpr
    inline float gAcceleration = 70.f;
    inline float gGroundSpeed = 20.f;
    inline int gGroundNumberOfAccelFrame = 4;
    inline int gGroundNumberOfSlowFrame = 4;
    inline float gAirSpeed = 20.f; // m/s
    inline int gAirNumberOfAccelFrame = 10;
    inline int gAirNumberOfSlowFrame = 10;
    inline float gAirMaxFallSpeed = -50.f; // m/s
    inline float gJumpImpulse = 24.f; // m/s
    inline float gWallFriction = 4.f; // m/s
    inline float gDoubleJumpFactor = 1.3f; // m/s
    inline float gGrappleBaseImpulse = 40.f;
    inline float gRopeDistanceJointFactor = 1.1f;
    //static constexpr double gAirControlAcceleration = 12.; // m/s
    //constexpr double gAcceleration = 10.;
    //constexpr double gPlayerJumpImpulse = 10.; // m/s
    //constexpr double gPlayerWalkingSpeed = 7.;
    //constexpr double gPlayerGroundFriction = 0.25;

} // namespace player


namespace render
{
    constexpr float gViewedHeight = 50.f;
    extern const GLfloat gSpritePixelWorldSize;
    constexpr GLsizei gBezierSubdivisions = 30;
} // namespace render


namespace rope
{
    inline std::vector<grapito::Position2> grappleVertices{
        grapito::Position2{0.f, 0.f}, grapito::Position2{0.5f, 0.f},
        grapito::Position2{0.7f, 0.2f}, grapito::Position2{0.55f, 0.55f},
        grapito::Position2{0.2f, 0.7f}, grapito::Position2{0.f, 0.5f}};
    constexpr GLfloat ropeWidth = .2f;
    constexpr GLfloat ropeHalfwidth = ropeWidth / 2 ;
    constexpr GLfloat curveTension = 0.f; // I.e. Catmull Rom
}


namespace sight
{
    constexpr float gReachDistance = 20.f;
    constexpr math::Degree<float> gAngleTolerance{40.f};

    constexpr float gBaseOutlineOffset = 0.25f;
} // namespace sight

namespace splash
{
    constexpr int gVerticalResolution = 800;
    constexpr float gHiddenDuration = 0.2f;
    constexpr float gLinearDuration = 0.6f;
    constexpr float gConstantDuration = 0.8f;
    constexpr float gDuration = 2 * (gHiddenDuration + gLinearDuration) + gConstantDuration;
} // namespace splash


} // namespace grapito 
} // namespace ad
