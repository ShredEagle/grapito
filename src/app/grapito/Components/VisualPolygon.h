#pragma once

#include "commons.h"
#include "math/Color.h"
#include "math/Matrix.h"
#include <aunteater/Component.h>
#include <graphics/commons.h>


namespace ad {
namespace grapito
{


struct VisualPolygon : public aunteater::Component<VisualPolygon>
{
    explicit VisualPolygon(std::vector<Position2> & aVertices, math::sdr::Rgb aColor = math::sdr::gWhite) :
        mVertices{aVertices},
        color{aColor}
    {}

    explicit VisualPolygon(std::vector<Position2> & aVertices, math::sdr::Rgb aColor, math::Matrix<3, 3, float> aTransform) :
        mVertices{std::move(aVertices)},
        color{aColor},
        transform{aTransform}
    {}

    std::vector<Position2> mVertices;
    math::sdr::Rgb color;
    math::Matrix<3, 3, float> transform{math::Matrix<3, 3, float>::Identity()};
};



} // namespace grapito
} // namespace ad
