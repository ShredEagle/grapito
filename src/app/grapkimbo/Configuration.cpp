#include "Configuration.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {

constexpr float gGigantismDampeningFactor = 2.f;


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
} // namespace game


namespace hud
{
    extern const float gViewedHeight = menu::gViewedHeight;
    extern const char * const gFont = menu::gFont;
    extern const GLfloat gTextHeight = menu::gTextHeight * 2;
}


namespace player
{
    const int gAirborneJumps = 1;
    const Vec2 gCameraGuideOffset{0.f, 15.f};
    const float gCameraGuideWeight = 1.f;
    const std::array<float, 2> gCameraLimits{7.f, -2.5f};
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