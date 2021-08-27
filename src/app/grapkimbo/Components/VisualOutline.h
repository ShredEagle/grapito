#pragma once

#include <aunteater/Component.h>


namespace ad {


struct VisualOutline : public aunteater::Component<VisualOutline>
{
    VisualOutline(Color aColor, double aOvershoot) :
        color{aColor},
        overshoot{aOvershoot, aOvershoot}
    {}

    Color color;
    Vec2<double> overshoot;
};



} // namespace ad
