#include "RopeUtilities.h"

#include "../commons.h"
#include "../configuration.h"

#include "../Components/Position.h"

#include <math/Curves/CardinalCubic.h>


namespace ad {
namespace grapito {


math::Position<3, GLfloat> getSegmentKnotPosition(aunteater::weak_entity aSegment)
{
    return {
        static_cast<math::Position<2, GLfloat>>(aSegment->get<Position>().position + Vec2{0., rope::ropeHalfwidth}),
        0.f
    };
}


void appendRopeSpline(const RopeCreator & aRope, std::back_insert_iterator<Spline> aOutputIterator)
{
    // TODO could be lazily evaluated instead of assigning a vector of Position2
    // Reserve the initial point extension.
    std::vector<math::Position<3,GLfloat>> knots{math::Position<3,GLfloat>::Zero()};
    std::transform(
        aRope.mRopeSegments.begin(), aRope.mRopeSegments.end(), std::back_inserter(knots),
        &getSegmentKnotPosition);

    // A cardinal cubic spline draws the curve between all points except first and last
    // Extend it
    // Note: there is an initial small rope segment, we rely on it to be enough not to extend
    // on the tail side.
    if (aRope.mRopeSegments.size() > 1)
    {
        knots.front() = knots.at(1) + (knots.at(1) - knots.at(2));
    }

    Spline result;
    for (int segment = 0; (segment + 3) < knots.size(); ++segment)
    {
        *(aOutputIterator++) = {
            math::CardinalCubic<3, GLfloat>{
                rope::curveTension,
                knots[segment],
                knots[segment + 1],
                knots[segment + 2],
                knots[segment + 3]
            }.toBezier(),
            rope::ropeHalfwidth
        };
    }
}


} // namespace grapito
} // namespace ad
