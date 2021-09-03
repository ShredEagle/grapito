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
                      math::Size<2, double> aDimension = math::Size<2, double>{1., 1.}) :
        position{std::move(aPosition)},
        oldPosition(position),
        dimension{std::move(aDimension)}
    {}

    Position2 center() const
    { return position + dimension.as<math::Vec>() / 2.; }

    math::Rectangle<double> rectangle() const
    { return {position, dimension}; } 

    Position2 position;
    math::Size<2, double> dimension;
    Position2 oldPosition;
};


} // namespace grapito
} // namespace ad
