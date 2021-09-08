#pragma once

#include "engine/commons.h"
#include "math/Color.h"
#include "math/Matrix.h"
#include <aunteater/Component.h>


namespace ad {
namespace grapito
{


struct VisualRectangle : public aunteater::Component<VisualRectangle>
{
    explicit VisualRectangle(Color aColor = math::sdr::gWhite, math::Matrix<3, 3, float> aTransform = math::Matrix<3, 3, float>::Identity()) :
        color{aColor},
        transform{aTransform}
    {}

    Color color;
    math::Matrix<3, 3, float> transform;
};



} // namespace grapito
} // namespace ad
