#pragma once

#include <aunteater/Timer.h>
#include <math/Matrix.h>
#include <math/Vector.h>

namespace ad
{
namespace grapito
{

typedef math::Vec<2, float> Vec2;
typedef math::Vec<3, float> Vec3;
typedef math::Position<2, float> Position2;
typedef math::Position<3, double> Position3;
using GrapitoTimer = aunteater::Timer_base<float>;

static inline float twoDVectorCross(Vec2 v, Vec2 w)
{
    return v.x() * w.y() - v.y() * w.x();
}

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
