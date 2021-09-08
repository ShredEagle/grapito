#pragma once

#include "Utils/HomogeneousTransformation.h"
#include "commons.h"

#include "Utils/DrawDebugStuff.h"
#include "math/Vector.h"
#include "renderer/commons.h"
#include <limits>
#include <math.h>
#include <math/Rectangle.h>

namespace ad {
namespace grapito
{

    // TODO right now we only handle rectangle collision box
    // This should handle convex polygon collision box in the future
    // And this value will then become a member of the collision box
#define EDGE_COUNT 4;

    struct Edge
    {
        Position2 origin;
        Position2 end;
        Vec2 normal;
    };

    class CollisionBox
    {

        public:
            //In 2D face count and vertex count are always the same
            math::Rectangle<double> mBox;
            int mFaceCount;

            CollisionBox(math::Rectangle<double> aBox = math::Rectangle<double>{{0.f, 0.f}, {1.f, 1.f}});
            const Position2 getVertice(const int index) const;
            const Position2 getVertice(const int index, const math::Radian<double> & theta, const Position2 & massCenter) const;
            const Edge getEdge(const int index) const;
            const Edge getEdge(const int index, const math::Radian<double> & theta, const Position2 & massCenter) const;
            const Position2 getSupport(const Vec2 direction, const math::Radian<double> & theta, const Position2 & massCenter) const;
            const Position2 getSupport(const Vec2 direction) const;
    };

} // namespace grapito
} // namespace ad
