#pragma once

#include "commons.h"
#include "engine/commons.h"
#include <aunteater/Component.h>


namespace ad {
namespace grapito
{


struct VisualOutline : public aunteater::Component<VisualOutline>
{
    VisualOutline(math::sdr::Rgb aColor, float aOvershoot) :
        color{aColor},
        overshoot{aOvershoot, aOvershoot}
    {}

    math::sdr::Rgb color;
    Vec2 overshoot;
};



} // namespace grapito
} // namespace ad
