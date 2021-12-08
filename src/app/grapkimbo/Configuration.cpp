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
} // namespace player


namespace render
{
    constexpr int horizontalSpriteScreenResolution = 800;
    const GLfloat gSpritePixelWorldSize = gViewedHeight / horizontalSpriteScreenResolution;
} // namespace render


namespace spriteanimation
{
    // The ASE animations are expressed in milliseconds
    const float gDefaultSpeed = 1000.f;
} // namespace splash


} // namespace grapito 
} // namespace ad