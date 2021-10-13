#include "RopeUtilities.h"

#include "../commons.h"
#include "../Configuration.h"
#include "../Entities.h"

#include "../Components/Body.h"
#include "../Components/Position.h"

#include <math/Curves/CardinalCubic.h>


namespace ad {
namespace grapito {


math::Position<3, GLfloat> getSegmentKnotPosition(aunteater::weak_entity aSegment,
                                                  Position2 aOffsetLocal)
{
    return {
        static_cast<math::Position<2, GLfloat>>(
            getLocalPointInWorld(aSegment->get<Body>(),
                                 aSegment->get<Position>(),
                                 aOffsetLocal)),
        0.f};
}


math::Position<3, GLfloat> getSegmentKnotPositionFront(aunteater::weak_entity aSegment)
{
    return getSegmentKnotPosition(aSegment, {0.f, rope::ropeHalfwidth});
}


void appendRopeSpline(const RopeCreator & aRope,
                      std::back_insert_iterator<Spline> aOutputIterator)
{
    // TODO could be lazily evaluated instead of assigning a vector of Position2
    // Reserve the initial point extension.
    std::vector<math::Position<3,GLfloat>> knots{math::Position<3,GLfloat>::Zero()};
    std::transform(
        aRope.mRopeSegments.begin(), aRope.mRopeSegments.end(), std::back_inserter(knots),
        &getSegmentKnotPositionFront);

    // A cardinal cubic spline draws the curve between all points except first and last
    // The beginning of the curve is on the grapple side (opposite of player)
    if (aRope.mRopeSegments.size() > 1)
    {
        // project the second point back accross the first
        knots.front() = knots.at(1) + (knots.at(1) - knots.at(2));
        auto previousKnot = knots.back();
        // Add a point representing the end of the last segment (cannot use player, if rope is detached)
        knots.push_back(getSegmentKnotPosition(
            aRope.mRopeSegments.back(), 
            {aRope.mRopeSegments.back()->get<Position>().dimension.width(), rope::ropeHalfwidth}));
        // project the second to last point across the list point
        knots.push_back(knots.back() + (knots.back() - previousKnot));
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
