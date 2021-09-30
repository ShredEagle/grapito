#pragma once

#include "math/Matrix.h"
#include <math/Vector.h>

namespace ad
{
namespace grapito
{
typedef math::Vec<2, double> Vec2;
typedef math::Position<2, double> Position2;

static inline double twoDVectorCross(Vec2 v, Vec2 w)
{
    return v.x() * w.y() - v.y() * w.x();
}

// Solve m * x = v
static inline Vec2 solveMatrix(math::Matrix<2, 2, double> m, Vec2 v)
{
    double det = m.at(0, 0) * m.at(1, 1) - m.at(0, 1) * m.at(1,0);

    if (det != 0.)
    {
        det = 1. / det;
    }

    return {
        det * (m.at(1,1) * v.x() - m.at(0,1) * v.y()),
        det * (m.at(0,0) * v.y() - m.at(1,0) * v.x())
    };
}

}
}
