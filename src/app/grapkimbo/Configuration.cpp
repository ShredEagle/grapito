#include "Configuration.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {

constexpr float gGigantismDampeningFactor = 2.f;


namespace background
{
    const std::string gSpaceImage = "images/backgrounds/Dynamic Space Background FREE/Nebula Aqua-Pink.png";
    const float gSpaceScrollFactor = 5.f;
    const std::string gSmallStarImage = "images/backgrounds/Dynamic Space Background FREE/Stars Small_1.png";
    const float gSmallStarScrollFactor = 7.5f;
    const std::string gStarImage = "images/backgrounds/Dynamic Space Background FREE/Stars-Big_1_1_PC.png";
    const float gStarScrollFactor = 10.f;
} // namespace background


namespace camera
{
    const float gCompetitorGuideFadeOutDuration = 1.5f;
} // namespace camera


namespace debug
{
    const float gCrossSize = 3.f;
    const math::sdr::Rgb gDirectControlDrawColor = math::sdr::gBlue;
    const Size2 gDirectControlDrawSize{gCrossSize, gCrossSize};
    const float gDirectControlSpeed = 50.f;
    const float gStepTimeIncrement = 0.016f;
} // namespace debug


namespace game
{
    const math::Size<2, int> gAppResolution{1600, 900};
    const float gCompetitorEliminationDistance = 15.f + (render::gViewedHeight / 2.f);
    const math::hdr::Rgb gClearColor{0.1, 0.2, 0.3};
    const float gCongratulationPhaseDuration = 2.5f;
    const Position2 gCongratulationScreenPosition{0.f, -50.f};
    const float gCountdownStepPeriod = 0.8f;
    const float gFadeDuration = 0.75f;
    const std::string gFallbackLanguage{"en"};
    const math::sdr::Rgb gFadeColor = math::sdr::gBlack;
    extern const float FloorBecomesLavaHeight = 8.f;
} // namespace game


namespace hud
{
    const float gViewedHeight = menu::gViewedHeight;
    const char * const gFont = menu::gFont;
    const GLfloat gSmallTextHeight = menu::gTextHeight * 1.5f;
    const GLfloat gLargeTextHeight = menu::gTextHeight  * 4;

    const Position2 gAltimeterPosition{0.f, -420.f};
    const Position2 gBestScorePosition{-600.f, -420.f};
    const Position2 gCountdownPosition{0.f, 20.f};
    const Position2 gModeTextPosition{-500.f, 300.f};

    const float gClimbMessageDuration = 1.5f;
    const float gInstructionMessageDuration = 2.f;

    const math::sdr::Rgb gTextColor = math::sdr::gWhite;
}


namespace menu
{
    const int gBlurringPasses = 6;
    const float gViewedHeight = 1000.f;
    const char * gFont = "fonts/Titillium_Web/TitilliumWeb-Bold.ttf";
    const math::sdr::Rgba gTextColor = math::sdr::gWhite;
    const GLfloat gTextHeight = 0.65f * gButtonSize.height();
    const float gButtonSpacing = 100.f;
    const math::sdr::Rgb gButtonColor{ 120, 100, 110 };
    const math::sdr::Rgb gSelectedColor{ 190, 190, 80 };
    const float gTransitionDuration = 0.275f;
} // namespace menu

namespace physic
{
    const int gMaxVelocityConstraintIteration = 200;
    const int gMaxPositionConstraintIteration = 200;
}


namespace player
{
    const int gAirborneJumps = 1;
    const Vec2 gCameraGuideOffset{0.f, 15.f};
    const float gCameraGuideWeight = 1.f;
    const std::array<float, 2> gCameraLimits{7.f, -2.5f};
    const float gDetachSpeedBoostFactor = 1.5f;
    const float gIdleSpeedLimit = 1;
    const math::Size<2, float> gSize = math::Size<2, float>{5.6f, 7.5f} / gGigantismDampeningFactor;
    const math::Size<2, float> gGrappleSpritePixelSize{30.f, 30.f};
    const float gGrappleFriction = 0.5f;
    float gGrappleDistanceJointFactor = 1.5f;
    float gRopeDistanceJointFactor = 1.1f;

    const math::sdr::Rgb gBlueColor{63, 63, 116};
    const math::sdr::Rgb gGreenColor{52, 212, 136};
    const math::sdr::Rgb gRedColor{235, 68, 53};
    const math::sdr::Rgb gYellowColor{192, 192, 139};
    const float gDoubleJumpCooldown = 15;
    const float gGrappleAnchoredCooldown = 30;
} // namespace player


namespace render
{
    constexpr int horizontalSpriteScreenResolution = 600 * gGigantismDampeningFactor;
    const GLfloat gSpritePixelWorldSize = gViewedHeight / horizontalSpriteScreenResolution;
    const GLfloat gSpriteOpacity = 1.f;
} // namespace render


namespace rope
{
    const math::hdr::Rgba gColor{0.76f, 0.55f, 0.43f};
} // namespace rope


namespace spriteanimation
{
    // The ASE animations are expressed in milliseconds
    const float gDefaultSpeed = 1000.f;
} // namespace splash


} // namespace grapito 
} // namespace ad
