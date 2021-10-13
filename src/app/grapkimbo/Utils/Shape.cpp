
#include "Shape.h"
#include "Utils/DrawDebugStuff.h"
#include "math/Matrix.h"

namespace ad {
namespace grapito {


Shape::Shape(math::Rectangle<float> aRectangle) :
    mFaceCount{4}
{
    Vec2 xVec{aRectangle.width(), 0.f};
    Vec2 yVec{0.f, aRectangle.height()};

    for (size_t index = 0; index < mFaceCount; ++index)
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

const math::Rectangle<float> Shape::getAABB() const
{
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = -std::numeric_limits<float>::max();
    float maxY = -std::numeric_limits<float>::max();

    for (size_t index = 0; index < mFaceCount; ++index)
    {
        const auto pos = getVertice(index);
        const float x = pos.x();
        const float y = pos.y();

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);

        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    return {{minX, minY}, {maxX - minX, maxY -minY}};
}

void Shape::debugRender()
{
    for (size_t i = 0; i < mFaceCount; ++i)
    {
        debugDrawer->drawLine({
                getVertice(i),
                getVertice((i + 1) % mFaceCount),
                0.,
                {255,0,255}
                });
    }
}

std::ostream &operator<<(std::ostream & os, const Shape & shape)
{
    auto aabb = shape.getAABB();
    os << "{ Edge\n";
    os << "    aabb : {\n";
    os << "        x : " << aabb.mPosition.x() << "\n";
    os << "        y : " << aabb.mPosition.y() << "\n";
    os << "        dimX : " << aabb.mDimension.width() << "\n";
    os << "        dimY : " << aabb.mDimension.height() << "\n";
    os << "    }\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const Shape::Edge & edge)
{
    os << "{ Edge\n";
    os << "    origin : " << edge.origin << "\n";
    os << "    end : " << edge.end << "\n";
    os << "    normal : " << edge.normal << "\n";
    os << "}\n";
    return os;
}

}
}
