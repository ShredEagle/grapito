#include "CollisionBox.h"
#include "Utils/DrawDebugStuff.h"
#include "math/Angle.h"
#include <iostream>

namespace ad {
namespace grapito {
CollisionBox::CollisionBox(math::Rectangle<double> aBox) : 
    mBox{aBox},
    mFaceCount{4}
{
};

// TODO Due to this and the normal calculation 
// we should not accept collision with negative width or height
// This insures that edge are in trigo orientation
// and makes normal computation easy
const Position2 CollisionBox::getVertice(const int index, const math::Radian<double> & theta, const Position2 & massCenter) const
{
    Vec2 xVec{mBox.width(), 0.};
    Vec2 yVec{0., mBox.height()};

    auto vertice = mBox.origin() + ((index & 0x01) ^ index >> 1) * xVec + (index >> 1) * yVec;


    return transformPosition(vertice, theta, massCenter);
}

const Position2 CollisionBox::getVertice(const int index) const
{
    return getVertice(index, math::Radian<double>{0.}, {0., 0.});
}

const Edge CollisionBox::getEdge(const int index, const math::Radian<double> & theta, const Position2 & massCenter) const
{
    Position2 origin = getVertice(index, theta, massCenter);
    Position2 end = getVertice((index + 1) % (mFaceCount), theta, massCenter);

    Vec2 edgeVector = end - origin;

    Vec2 normal{edgeVector.y(), -edgeVector.x()};

    return {origin, end, normal.normalize()};
};

const Edge CollisionBox::getEdge(const int index) const
{
    return getEdge(index, math::Radian<double>{0.}, {0., 0.});
}


const Position2 CollisionBox::getSupport(const Vec2 direction, const math::Radian<double> & theta, const Position2 & massCenter) const
{
    double bestProjection = -std::numeric_limits<double>::max();
    Position2 bestVertex{0.f, 0.f};

    //In 2D face count and vertex count are always the same
    for (int i = 0; i < mFaceCount; ++i)
    {
        Position2 vertex = getVertice(i, theta, massCenter);
        double projection = direction.dot(vertex.as<math::Vec>());

        if (projection > bestProjection)
        {
            bestProjection = projection;
            bestVertex = vertex;
        }
    }

    return bestVertex;
}

const Position2 CollisionBox::getSupport(const Vec2 direction) const
{
    return getSupport(direction, math::Radian<double>{0.}, {0., 0.});
}
}
}
