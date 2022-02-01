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


namespace background
{
    extern const std::string gSpaceImage;
    extern const float gSpaceScrollFactor;
    extern const std::string gSmallStarImage;
    extern const float gSmallStarScrollFactor;
    extern const std::string gStarImage;
    extern const float gStarScrollFactor;
} // namespace background


namespace camera
{
    constexpr float gAnchorGuideFadeIn = 2.f;
    constexpr float gAnchorGuideFadeOut = 0.8f;

    // When a competitor is eliminated, the duration for its camera guide fade out.
    extern const float gCompetitorGuideFadeOutDuration;
}


namespace controller
{
    constexpr float gDeadzone = 0.8f;
    constexpr float gHorizontalDeadZone = 0.4f;
    constexpr float gVerticalDeadZone = 0.2f;
}


namespace debug
{
    extern const float gCrossSize;
    extern const math::sdr::Rgb gDirectControlDrawColor;
    extern const Size2 gDirectControlDrawSize;
    extern const float gDirectControlSpeed;
    extern const float gStepTimeIncrement;
};


namespace game
{
    extern const math::Size<2, int> gAppResolution;
    constexpr math::hdr::Rgb gClearColor{0.1, 0.2, 0.3};
    // If a competitor fall below this vertical limit, it is eliminated.
    extern const float gCompetitorEliminationDistance;
    extern const float gCongratulationPhaseDuration;
    extern const Position2 gCongratulationScreenPosition;
    extern const std::string gFallbackLanguage;
    constexpr int gMaxPlayerSlots = 5;
} // namespace game


namespace hud
{
    extern const float gViewedHeight;
    extern const char * const gFont;
    extern const GLfloat gTextHeight;
    extern const Position2 gModeTextPosition;
}


namespace menu
{
    constexpr int gBlurringPasses = 6;
    constexpr float gViewedHeight = 1000.f;
    constexpr math::Size<2, GLfloat> gButtonSize{400.f, 80.f};
    constexpr const char * gFont = "fonts/dejavu-fonts-ttf-2.37/ttf/DejaVuSans.ttf";
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
    extern const int gAirborneJumps;
    extern const Vec2 gCameraGuideOffset;
    extern const float gCameraGuideWeight;
    extern const std::array<float, 2> gCameraLimits;
    extern const float gDetachSpeedBoostFactor;
    constexpr float gMass = 80.f;
    extern const math::Size<2, float> gSize;
    constexpr float gInitialRopeLength = 3.f;
    constexpr math::Radian<float> gInitialAngle{math::pi<float>/3.f};
    //TODO(franz) move to impl file
    inline float gAcceleration = 70.f;
    inline float gGroundSpeed = 20.f;
    inline int gGroundNumberOfAccelFrame = 4;
    inline int gGroundNumberOfSlowFrame = 4;
    inline float gAirSpeed = 20.f; // m/s
    inline int gAirNumberOfAccelFrame = 10;
    inline int gAirNumberOfSlowFrame = 10;
    inline float gAirMaxFallSpeed = -50.f; // m/s
    inline float gJumpImpulse = 24.f; // m/s
    inline float gGrappleReleaseImpulse = 24.f; // m/s
    inline float gGrappleSwingSpeed = 5.f; // m/s
    inline float gWallFriction = 4.f; // m/s
    inline float gDoubleJumpFactor = 1.3f; // m/s
    inline float gGrappleBaseImpulse = 70.f;
    extern const float gGrappleFriction;
    extern float gRopeDistanceJointFactor;
    extern float gGrappleDistanceJointFactor;
    //static constexpr double gAirControlAcceleration = 12.; // m/s
    //constexpr double gAcceleration = 10.;
    //constexpr double gPlayerJumpImpulse = 10.; // m/s
    //constexpr double gPlayerWalkingSpeed = 7.;
    //constexpr double gPlayerGroundFriction = 0.25;

    extern const float gIdleSpeedLimit;

} // namespace player


namespace render
{
    constexpr float gViewedHeight = 50.f;
    extern const GLfloat gSpritePixelWorldSize;
    constexpr GLsizei gBezierSubdivisions = 30;
    extern const GLfloat gSpriteOpacity;
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


namespace spriteanimation
{
    extern const float gDefaultSpeed;
} // namespace spriteanimation

} // namespace grapito
} // namespace ad
