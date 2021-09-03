#pragma once

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
            float theta;
            Position2 massCenter;

            CollisionBox(math::Rectangle<double> aBox = math::Rectangle<double>{{0.f, 0.f}, {1.f, 1.f}}, float aTheta = 0.) : 
                mBox{aBox},
                mFaceCount{4},
                massCenter{Position2::Zero()},
                theta{aTheta}
            {
                Vec2 vecMassCenter = Vec2::Zero();
                for (int i = 0; i < mFaceCount; ++i)
                {
                    vecMassCenter += (getVertice(i).as<math::Vec>() / mFaceCount);
                }
                massCenter = static_cast<Position2>(vecMassCenter);
            };

            // TODO Due to this and the normal calculation 
            // we should not accept collision with negative width or height
            // This insures that edge are in trigo orientation
            // and makes normal computation easy
            const Position2 getVertice(const int index) const
            {
                Vec2 xVec{mBox.width() * cos(theta), mBox.width() * sin(theta)};
                Vec2 yVec{-mBox.width() * sin(theta), mBox.height() * cos(theta)};

                return mBox.origin() + ((index & 0x01) ^ index >> 1) * xVec + (index >> 1) * yVec;
            }

            const Edge getEdge(const int index) const
            {
                Position2 origin = getVertice(index);
                Position2 end = getVertice((index + 1) % (mFaceCount));

                Vec2 edgeVector = end - origin;

                Vec2 normal{edgeVector.y(), -edgeVector.x()};

                return {origin, end, normal.normalize()};
            };

            const Position2 getSupport(const Vec2 direction) const
            {
                double bestProjection = -std::numeric_limits<double>::max();
                Position2 bestVertex{0.f, 0.f};

                //In 2D face count and vertex count are always the same
                for (int i = 0; i < mFaceCount; ++i)
                {
                    Position2 vertex = getVertice(i);
                    double projection = direction.dot(vertex.as<math::Vec>());

                    if (projection > bestProjection)
                    {
                        bestProjection = projection;
                        bestVertex = vertex;
                    }
                }

                return bestVertex;
            }
    };

} // namespace grapito
} // namespace ad
