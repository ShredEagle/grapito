#pragma once

#include "commons.h"
#include "math/Matrix.h"
#include <math/Angle.h>
#include <math/Homogeneous.h>

namespace ad {
namespace grapito {


inline math::Matrix<3, 3, double> createPrefixedTransform(math::Radian<double> angle, Position2 center)
{
    double x = center.x();
    double y = center.y();
    return { 
        cos(angle),                            sin(angle),                           0.,
        -sin(angle),                           cos(angle),                           0.,
        -x * cos(angle) + y * sin(angle) + x,  -x * sin(angle) - y * cos(angle) + y, 1.
    };
}


inline Position2 transformPosition(Position2 position, math::Matrix<3, 3, double> transform)
{
    math::Position<3, double> homoPos = math::homogeneous::makePosition(position) * transform;
    return {homoPos.x(), homoPos.y()};
}


inline Position2 transformPosition(Position2 position, math::Radian<double> angle, Position2 center)
{
    double x = center.x();
    double y = center.y();
    auto transformMatrix = math::Matrix<3, 3, double>{ 
        cos(angle),                            sin(angle),                           0.,
        -sin(angle),                           cos(angle),                           0.,
        -x * cos(angle) + y * sin(angle) + x,  -x * sin(angle) - y * cos(angle) + y, 1.
    };

    return transformPosition(position, transformMatrix);
}


inline Vec2 transformVector(Vec2 aVector, math::Matrix<3, 3, double> transform)
{
    math::Vec<3, double> homoPos = math::homogeneous::makeVec(aVector) * transform;
    return {homoPos.x(), homoPos.y()};
}


inline Vec2 transformVector(Vec2 aVector, math::Radian<double> angle)
{
    auto transformMatrix = math::Matrix<3, 3, double>{
        cos(angle), sin(angle), 0.,
        -sin(angle), cos(angle), 0.,
        0.,          0.,           1.
    };

    return transformVector(aVector, transformMatrix);
}

}
}
