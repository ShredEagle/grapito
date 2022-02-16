#pragma once

#include "math/Color.h"
#include "math/Matrix.h"
#include <aunteater/Component.h>
#include <graphics/commons.h>


namespace ad {
namespace grapito
{


struct VisualRectangle : public aunteater::Component<VisualRectangle>
{
    enum class Scope
    {
        Gameplay,
        RopeStructure,
    };

    explicit VisualRectangle(math::sdr::Rgb aColor = math::sdr::gWhite) :
        color{aColor}
    {}

    explicit VisualRectangle(math::sdr::Rgb aColor, math::Matrix<3, 3, float> aTransform) :
        color{aColor},
        transform{aTransform}
    {}

    VisualRectangle(math::sdr::Rgb aColor, Scope aScope) :
        color{aColor},
        scope{aScope}
    {}

    math::sdr::Rgb color;
    math::Matrix<3, 3, float> transform{math::Matrix<3, 3, float>::Identity()};
    Scope scope{Scope::Gameplay};
};



} // namespace grapito
} // namespace ad
