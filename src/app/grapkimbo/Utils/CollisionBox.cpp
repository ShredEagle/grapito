#include "CollisionBox.h"
#include "Utils/DrawDebugStuff.h"
#include "Utils/Shape.h"
#include "commons.h"
#include "math/Angle.h"
#include <iostream>

namespace ad {
namespace grapito {
CollisionBox::CollisionBox(Shape aShape) : 
    shape(std::move(aShape))
{};

CollisionBox::CollisionBox(math::Rectangle<double> aRectangle) : 
    shape(aRectangle)
{};


// TODO Due to this and the normal calculation 
// we should not accept collision with negative width or height
// This insures that edge are in trigo orientation
// and makes normal computation easy
const Position2 CollisionBox::getSupport(const Vec2 direction) const
{
    double bestProjection = -std::numeric_limits<double>::max();
    Position2 bestVertex{0.f, 0.f};

    //In 2D face count and vertex count are always the same
    for (int i = 0; i < shape.mFaceCount; ++i)
    {
        Position2 vertex = shape.getVertice(i);
        double projection = direction.dot(vertex.as<math::Vec>());

        if (projection > bestProjection)
        {
            bestProjection = projection;
            bestVertex = vertex;
        }
    }

    return bestVertex;
}

std::ostream &operator<<(std::ostream & os, const CollisionBox & box)
{
    //return os << "{ CollisionBox\n    Box : " << box.mBox.mPosition << " " << box.mBox.mDimension << "\n    Face count : " << box.mFaceCount << "\n}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const Edge & edge)
{
    return os << "{ Edge\n    origin : " << edge.origin << "\n    end : " << edge.end << "\n    normal : " << edge.normal << "\n}\n";
}

}
}
