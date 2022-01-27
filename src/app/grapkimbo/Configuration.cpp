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
    const float gCongratulationPhaseDuration = 3.f;
    const Position2 gCongratulationScreenPosition{-200.f, -100.f};
    const std::string gFallbackLanguage{"en"};
} // namespace game


namespace hud
{
    const float gViewedHeight = menu::gViewedHeight;
    const char * const gFont = menu::gFont;
    const GLfloat gTextHeight = menu::gTextHeight  * 2.5 ;
    const Position2 gModeTextPosition{-500.f, 300.f};
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
    const float gGrappleFriction = 0.5f;
} // namespace player


namespace render
{
    constexpr int horizontalSpriteScreenResolution = 600 * gGigantismDampeningFactor;
    const GLfloat gSpritePixelWorldSize = gViewedHeight / horizontalSpriteScreenResolution;
    const GLfloat gSpriteOpacity = 1.f;
} // namespace render


namespace spriteanimation
{
    // The ASE animations are expressed in milliseconds
    const float gDefaultSpeed = 1000.f;
} // namespace splash


} // namespace grapito 
} // namespace ad