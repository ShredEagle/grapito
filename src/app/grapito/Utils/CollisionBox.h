#pragma once

#include "Utils/HomogeneousTransformation.h"
#include "Utils/Shape.h"
#include "commons.h"

#include "Utils/DrawDebugStuff.h"
#include "math/Vector.h"
#include "renderer/commons.h"
#include <limits>
#include <math.h>
#include <math/Rectangle.h>
#include <vector>

namespace ad {
namespace grapito
{

    // TODO right now we only handle rectangle collision box
    // This should handle convex polygon collision box in the future
    // And this value will then become a member of the collision box

    class CollisionBox
    {

        public:
            //In 2D face count and vertex count are always the same
            Shape shape;

            CollisionBox(Shape aShape);
            CollisionBox(math::Rectangle<float> aRectangle);
            CollisionBox(std::vector<Position2> aVertices);

            const Position2 getSupport(const Vec2 direction) const;

            friend std::ostream & operator<<(std::ostream & os, const CollisionBox & box);
    };

} // namespace grapito
} // namespace ad
