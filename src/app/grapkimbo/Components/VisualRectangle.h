#pragma once

#include "engine/commons.h"
#include "math/Color.h"
#include <aunteater/Component.h>


namespace ad {


struct VisualRectangle : public aunteater::Component<VisualRectangle>
{
    explicit VisualRectangle(Color aColor = math::sdr::gWhite, float aAngle = 0.) :
        color{aColor},
        angle{aAngle}
    {}

    Color color;
    math::Radian<float> angle;
};



} // namespace ad
