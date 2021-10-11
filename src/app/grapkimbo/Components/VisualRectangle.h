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
    enum class Scope
    {
        Gameplay,
        RopeStructure,
    };

    explicit VisualRectangle(Color aColor = math::sdr::gWhite) :
        color{aColor}
    {}

    explicit VisualRectangle(Color aColor, math::Matrix<3, 3, float> aTransform) :
        color{aColor},
        transform{aTransform}
    {}

    VisualRectangle(Color aColor, Scope aScope) :
        color{aColor},
        scope{aScope}
    {}

    Color color;
    math::Matrix<3, 3, float> transform{math::Matrix<3, 3, float>::Identity()};
    Scope scope{Scope::Gameplay};
};



} // namespace grapito
} // namespace ad
