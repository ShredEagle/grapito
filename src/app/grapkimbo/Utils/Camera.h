#pragma once


#include "../commons.h"
#include "../Configuration.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {


inline math::Rectangle<GLfloat> getViewedRectangle(Position2 aCameraPosition, float aViewportRatio)
{
    return math::Rectangle<GLfloat>{
        aCameraPosition,
        math::makeSizeFromHeight(render::gViewedHeight, aViewportRatio)
    }.centered();
}


inline Vec2 weightedGuideContribution(const Position & aGeometry, const CameraGuide & aCameraGuide)
{
    Position2 guidePosition = aGeometry.position + aCameraGuide.offset;
    return guidePosition.as<math::Vec>() * aCameraGuide.influence;
}

} // namespace grapito
} // namespace ad
