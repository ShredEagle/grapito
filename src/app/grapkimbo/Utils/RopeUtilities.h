#pragma once


#include "../Components/RopeCreator.h"

#include <graphics/Curving.h>

#include <math/Curves/Bezier.h>

#include <vector>


namespace ad {
namespace grapito {


using Spline = std::vector<graphics::Curving::Instance>;
void appendRopeSpline(const RopeCreator & aRope,
                      std::back_insert_iterator<Spline> aOutputIterator);


} // namespace grapito
} // namespace ad
