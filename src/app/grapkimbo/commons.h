#pragma once

#include <aunteater/Timer.h>
#include <math/Matrix.h>
#include <math/Rectangle.h>
#include <math/Vector.h>

namespace ad
{
namespace grapito
{

using Vec2 = math::Vec<2, float>;
using Vec3 = math::Vec<3, float>;
using Position2 = math::Position<2, float>;
using Position3 = math::Position<3, double>;
using Rectangle = math::Rectangle<float>;

using GrapitoTimer = aunteater::Timer_base<float>;


/// RFP remove?
static inline float twoDVectorCross(Vec2 v, Vec2 w)
{
    return v.x() * w.y() - v.y() * w.x();
}


/// RFP remove?
// Solve m * x = v
static inline Vec2 solveMatrix(math::Matrix<2, 2, float> m, Vec2 v)
{
    float det = m.at(0, 0) * m.at(1, 1) - m.at(0, 1) * m.at(1,0);

    if (det != 0.)
    {
        det = 1.f / det;
    }

    return {
        det * (m.at(1,1) * v.x() - m.at(0,1) * v.y()),
        det * (m.at(0,0) * v.y() - m.at(1,0) * v.x())
    };
}

}
}
