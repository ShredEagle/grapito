#pragma once

#include "../commons.h"
#include "math/Constants.h"
#include <math/Vector.h>

namespace ad
{
namespace grapito
{
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

static inline Vec3 solveMatrix(math::Matrix<3, 3, float> m, Vec3 v)
{
    Vec3 firstCol = {m.at(0, 0), m.at(1,0), m.at(2,0)};
    Vec3 secondCol = {m.at(0, 1), m.at(1,1), m.at(2,1)};
    Vec3 thirdCol = {m.at(0, 2), m.at(1,2), m.at(2,2)};

    float det = firstCol.dot(secondCol.cross(thirdCol));

    if(det != 0.f)
    {
        det = 1.f / det;
    }

    return {
        det * v.dot(secondCol.cross(thirdCol)),
        det * firstCol.dot(v.cross(thirdCol)),
        det * firstCol.dot(secondCol.cross(v))
    };
}

static inline math::Matrix<3, 3, float> getInverse2by2(const math::Matrix<3, 3, float> & aMatrix)
{
    auto tempResultMatrix = aMatrix.getSubmatrix(1, 1).inverse();

    return math::Matrix<3, 3, float>{
        tempResultMatrix.at(0), tempResultMatrix.at(1), 0.f,
        tempResultMatrix.at(2), tempResultMatrix.at(3), 0.f,
        0.f, 0.f, 0.f
    };
}

static inline void computeStiffnessDamping(
        float & aStiffness,
        float & aDamping,
        float aFrequencyHertz,
        float aDampingRatio,
        float massA,
        float massB
        )
{
        float mass;

        if (massA > 0.f && massB > 0.f)
        {
            mass = massA * massB / (massA + massB);
        }
        else if (massA > 0.f)
        {
            mass = massA;
        }
        else
        {
            mass = massB;
        }

        float omega = 2.f * math::pi<float> * aFrequencyHertz;
        aStiffness = mass * omega * omega;
        aDamping = 2.f * mass * aDampingRatio * omega;
}

}
}
