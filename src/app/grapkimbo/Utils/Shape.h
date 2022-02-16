#pragma once

// a shape is a polygon
#include "commons.h"
#include "math/Rectangle.h"
#include <vector>

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
        Vec2 direction;

        friend std::ostream &operator<<(std::ostream & os, const Edge & edge);

    };

    Shape(std::vector<Position2> aVertices) :
        mFaceCount{aVertices.size()},
        vertices{std::move(aVertices)}
    {}

    Shape(math::Rectangle<float> aRectangle);

    inline const Position2 getVertice(const int index) const
    {
        return vertices.at(index);
    };
    const Edge getEdge(const int index) const;
    const math::Rectangle<float> getAABB() const;
    void debugRender();

    friend std::ostream &operator<<(std::ostream & os, const Shape & shape);


    size_t mFaceCount;
    std::vector<Position2> vertices;
};

}
}
