#pragma once

// a shape is a polygon
#include "commons.h"
#include "math/Rectangle.h"

namespace ad {
namespace grapito {

class Shape
{
public:
    struct Edge
    {
        Position2 origin;
        Position2 end;
        Vec2 normal;

        friend std::ostream &operator<<(std::ostream & os, const Edge & edge);

    };

    Shape(std::vector<Position2> aVertices) :
        vertices(std::move(aVertices)),
        mFaceCount(aVertices.size())
    {}

    Shape(math::Rectangle<double> aRectangle);

    const Position2 getVertice(const int index) const;
    const Edge getEdge(const int index) const;
    const math::Rectangle<double> getAABB() const;

    int mFaceCount;
    std::vector<Position2> vertices;
};

}
}
