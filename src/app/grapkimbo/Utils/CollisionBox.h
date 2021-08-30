#pragma once

#include "Utils/DrawDebugStuff.h"
#include <limits>
#include <math.h>
#include <math/Vector.h>
#include <math/Rectangle.h>

namespace ad {

    // TODO right now we only handle rectangle collision box
    // This should handle convex polygon collision box in the future
    // And this value will then become a member of the collision box
#define EDGE_COUNT 4;

    struct Edge
    {
        math::Position<2, double> origin;
        math::Position<2, double> end;
        math::Vec<2, double> normal;
    };

    class CollisionBox
    {

        public:
            //In 2D face count and vertex count are always the same
            math::Rectangle<double> mBox;
            int mFaceCount;
            float theta;
            math::Position<2, double> massCenter;

            CollisionBox(math::Rectangle<double> aBox = math::Rectangle<double>{{0.f, 0.f}, {1.f, 1.f}}, float aTheta = 0.) : 
                mBox{aBox},
                mFaceCount{4},
                massCenter{math::Position<2, double>::Zero()},
                theta{aTheta}
            {
                math::Vec<2, double> vecMassCenter = math::Vec<2, double>::Zero();
                for (int i = 0; i < mFaceCount; ++i)
                {
                    vecMassCenter += (getVertice(i).as<math::Vec>() / mFaceCount);
                }
                massCenter = static_cast<math::Position<2, double>>(vecMassCenter);
            };

            // TODO Due to this and the normal calculation 
            // we should not accept collision with negative width or height
            // This insures that edge are in trigo orientation
            // and makes normal computation easy
            const math::Position<2, double> getVertice(const int index) const
            {
                math::Vec<2, double> xVec{mBox.width() * cos(theta), mBox.width() * sin(theta)};
                math::Vec<2, double> yVec{-mBox.width() * sin(theta), mBox.height() * cos(theta)};

                return mBox.origin() + ((index & 0x01) ^ index >> 1) * xVec + (index >> 1) * yVec;
            }

            const Edge getEdge(const int index) const
            {
                math::Position<2, double> origin = getVertice(index);
                math::Position<2, double> end = getVertice((index + 1) % (mFaceCount));

                math::Vec<2, double> edgeVector = end - origin;

                math::Vec<2, double> normal{edgeVector.y(), -edgeVector.x()};

                return {origin, end, normal.normalize()};
            };

            const math::Position<2, double> getSupport(const math::Vec<2, double> direction) const
            {
                double bestProjection = -std::numeric_limits<double>::max();
                math::Position<2, double> bestVertex{0.f, 0.f};

                //In 2D face count and vertex count are always the same
                for (int i = 0; i < mFaceCount; ++i)
                {
                    math::Position<2, double> vertex = getVertice(i);
                    double projection = direction.dot(static_cast<math::Vec<2, double>>(vertex));

                    if (projection > bestProjection)
                    {
                        bestProjection = projection;
                        bestVertex = vertex;
                    }
                }

                return bestVertex;
            }
    };

} // namespace ad
