#pragma once

#include <aunteater/Component.h>


namespace ad {
namespace grapito
{


struct VisualOutline : public aunteater::Component<VisualOutline>
{
    VisualOutline(Color aColor, double aOvershoot) :
        color{aColor},
        overshoot{aOvershoot, aOvershoot}
    {}

    Color color;
    Vec2 overshoot;
};



} // namespace grapito
} // namespace ad
