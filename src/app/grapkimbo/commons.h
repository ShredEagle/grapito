#pragma once

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

}
}
