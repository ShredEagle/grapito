#include "Configuration.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {


namespace game
{
    const math::Size<2, int> gAppResolution{1600, 900};
} // namespace game


namespace player
{
    const float gIdleSpeedLimit = 1;
    const math::Size<2, float> gSize{5.6f, 7.5f};
} // namespace player


namespace render
{
    constexpr int horizontalSpriteScreenResolution = 600;
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