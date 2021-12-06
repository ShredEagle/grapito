#include "Configuration.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {


namespace game
{
    const math::Size<2, int> gAppResolution{1600, 900};
} // namespace game


namespace render
{
    constexpr int horizontalSpriteScreenResolution = 800;
    const GLfloat gSpritePixelWorldSize = gViewedHeight / horizontalSpriteScreenResolution;
} // namespace render


} // namespace grapito 
} // namespace ad