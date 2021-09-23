
#include "Shape.h"

namespace ad {
namespace grapito {


Shape::Shape(math::Rectangle<double> aRectangle) :
    mFaceCount{4}
{
    Vec2 xVec{aRectangle.width(), 0.};
    Vec2 yVec{0., aRectangle.height()};

    for (int index = 0; index < mFaceCount; ++index)
    {
        auto vertice = aRectangle.origin() + ((index & 0x01) ^ index >> 1) * xVec + (index >> 1) * yVec;
        vertices.emplace_back(vertice);
    }
}

const Position2 Shape::getVertice(const int index) const
{
    return vertices.at(index);
}

const Shape::Edge Shape::getEdge(const int index) const
{
    Position2 origin = getVertice(index);
    Position2 end = getVertice((index + 1) % (mFaceCount));

    Vec2 edgeVector = end - origin;

    Vec2 normal{edgeVector.y(), -edgeVector.x()};

    return {origin, end, normal.normalize(), edgeVector};
};

const math::Rectangle<double> Shape::getAABB() const
{
    double minX = -std::numeric_limits<double>::max();
    double minY = -std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::max();

    for (int index = 0; index < mFaceCount; ++index)
    {
        const auto pos = getVertice(index);
        const double x = pos.x();
        const double y = pos.y();

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);

        minY = std::min(minY, x);
        maxY = std::max(maxY, x);
    }

    return {{minX, minY}, {maxX - minX, maxY -minY}};
}

}
}
