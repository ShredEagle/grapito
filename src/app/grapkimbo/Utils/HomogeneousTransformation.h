#pragma once

#include "commons.h"
#include "math/Matrix.h"
#include <math/Angle.h>
#include <math/Homogeneous.h>
#include <math/Transformations.h>

namespace ad {
namespace grapito {


inline Position2 transformPosition(Position2 position, math::AffineMatrix<3, float> transform)
{
    math::Position<3, float> homoPos = math::homogeneous::makePosition(position) * transform;
    return {homoPos.x(), homoPos.y()};
}

inline Position2 transformPosition(Position2 position, math::Radian<float> angle, Position2 center)
{
    return transformPosition(position, math::trans2d::rotateAbout(angle, center));
}

inline Vec2 transformVector(Vec2 aVector, math::AffineMatrix<3, float> transform)
{
    math::Vec<3, float> homoPos = math::homogeneous::makeVec(aVector) * transform;
    return {homoPos.x(), homoPos.y()};
}

inline Vec2 transformVector(Vec2 aVector, math::Radian<float> angle)
{
    return transformVector(aVector, math::trans2d::rotate(angle));
}


}
}
