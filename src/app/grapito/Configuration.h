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

    // How much margin to take on left/right level limits
    extern const float gLateralLimitMargin;
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
    extern const math::hdr::Rgb gClearColor;
    // If a competitor fall below this vertical limit, it is eliminated.
    extern const float gCompetitorEliminationDistance;
    extern const float gCongratulationPhaseDuration;
    extern const Position2 gCongratulationScreenPosition;
    extern const float gCountdownStepPeriod;
    extern const float gFadeDuration;
    extern const std::string gFallbackLanguage;
    extern const math::sdr::Rgb gFadeColor;
    // TODO FP why 5 ?
    constexpr int gMaxPlayerSlots = 5;
    extern const float FloorBecomesLavaHeight;
} // namespace game


namespace hud
{
    extern const float gViewedHeight;
    extern const char * const gFont;
    extern const GLfloat gSmallTextHeight;
    extern const GLfloat gLargeTextHeight;

    extern const Position2 gAltimeterPosition;
    extern const Position2 gBestScorePosition;
    extern const Position2 gCountdownPosition;
    extern const Position2 gModeTextPosition;

    extern const float gClimbMessageDuration;
    extern const float gInstructionMessageDuration;

    extern const math::sdr::Rgb gTextColor;
}


namespace menu
{
    constexpr math::Size<2, GLfloat> gButtonSize{400.f, 80.f}; // kept in .h to avoid SIOF
    extern const int gBlurringPasses;
    extern const float gViewedHeight;
    extern const char * gFont;
    extern const math::sdr::Rgba gTextColor;
    extern const GLfloat gTextHeight;
    extern const float gButtonSpacing;
    extern const math::sdr::Rgb gButtonColor;
    extern const std::vector<math::sdr::Rgb> gSelectedColor;
    extern const float gTransitionDuration;

    extern const Position2 gVersionPosition;
    extern const math::sdr::Rgba gVersionTextColor;
}


namespace physic
{
    //This is the error that is allowed during physics resolution
    //This represent the separation that we expect at rest
    constexpr float gLinearSlop = 0.001f;
    constexpr float gAngularSlop = 2.f / 180 * math::pi<float>;
    constexpr float gBaumgarteFactor = 0.2f;
    constexpr float gMaxLinearCorrection = 0.2f;
    extern int gMaxVelocityConstraintIteration;
    extern int gMaxPositionConstraintIteration;
}


namespace player
{
    extern const math::Size<2, float> gSize;

    extern const int gAirborneJumps;
    extern const Vec2 gCameraGuideOffset;
    extern const float gCameraGuideWeight;
    extern const std::array<float, 2> gCameraLimits;
    extern const float gDetachSpeedBoostFactor;
    constexpr float gMass = 80.f;
    extern const math::Size<2, float> gGrappleSpritePixelSize;
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
    inline float gJumpImpulse = 25.f; // m/s
    inline float gGrappleReleaseImpulse = 24.f; // m/s
    inline float gGrappleSwingSpeed = 5.f; // m/s
    inline float gWallFriction = 4.f; // m/s
    inline float gDoubleJumpFactor = 1.3f; // m/s
    extern float gGrappleBaseImpulse;
    extern const float gGrappleFriction;
    extern float gRopeDistanceJointFactor;
    extern float gGrappleDistanceJointFactor;
    extern const float gDoubleJumpCooldown;
    extern const float gGrappleAnchoredCooldown;
    //static constexpr double gAirControlAcceleration = 12.; // m/s
    //constexpr double gAcceleration = 10.;
    //constexpr double gPlayerJumpImpulse = 10.; // m/s
    //constexpr double gPlayerWalkingSpeed = 7.;
    //constexpr double gPlayerGroundFriction = 0.25;

    extern const float gIdleSpeedLimit;

    extern const math::sdr::Rgb gBlueColor;
    extern const math::sdr::Rgb gGreenColor;
    extern const math::sdr::Rgb gRedColor;
    extern const math::sdr::Rgb gYellowColor;

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
    extern const std::vector<grapito::Position2> grappleVertices;
    constexpr GLfloat ropeWidth = .2f;
    constexpr GLfloat ropeHalfwidth = ropeWidth / 2 ;
    constexpr GLfloat curveTension = 0.f; // I.e. Catmull Rom
    extern const math::hdr::Rgba gColor;
}


namespace sight
{
    constexpr float gReachDistance = 20.f;
    constexpr math::Degree<float> gAngleTolerance{40.f};

    constexpr float gBaseOutlineOffset = 0.25f;
} // namespace sight

namespace splash
{
    constexpr int gVerticalResolution = 1440;
    constexpr float gHiddenDuration = 0.2f;
    constexpr float gLinearDuration = 0.8f;
    constexpr float gConstantDuration = 1.1f;
    constexpr float gDuration = 2 * (gHiddenDuration + gLinearDuration) + gConstantDuration;
} // namespace splash


namespace spriteanimation
{
    extern const float gDefaultSpeed;
} // namespace spriteanimation

} // namespace grapito
} // namespace ad
