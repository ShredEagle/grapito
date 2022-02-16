#pragma once

#include "commons.h"
#include <aunteater/Component.h>
#include <math/Rectangle.h>
#include <math/Vector.h>

namespace ad {
namespace grapito
{

struct Position : public aunteater::Component<Position>
{
    explicit Position(Position2 aPosition  = Position2::Zero(),
                      math::Size<2, float> aDimension = math::Size<2, float>{1.f, 1.f}) :
        position{std::move(aPosition)},
        dimension{std::move(aDimension)},
        oldPosition(position)
    {}

    Position2 center() const
    { return position + dimension.as<math::Vec>() / 2.f; }

    math::Rectangle<float> rectangle() const
    { return {position, dimension}; } 

    Position2 position;
    math::Size<2, float> dimension;
    Position2 oldPosition;
};


} // namespace grapito
} // namespace ad
