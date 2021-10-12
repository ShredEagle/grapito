#pragma once

#include <aunteater/Component.h>


namespace ad {
namespace grapito
{


struct VisualOutline : public aunteater::Component<VisualOutline>
{
    VisualOutline(math::sdr::Rgb aColor, double aOvershoot) :
        color{aColor},
        overshoot{aOvershoot, aOvershoot}
    {}

    math::sdr::Rgb color;
    Vec2 overshoot;
};



} // namespace grapito
} // namespace ad
