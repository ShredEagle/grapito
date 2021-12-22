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


} // namespace grapito
} // namespace ad
