#pragma once

#include <aunteater/Component.h>
#include <math/Vector.h>

namespace ad {

struct Position : public aunteater::Component<Position>
{
    explicit Position(math::Position<2, double> aPosition  = math::Position<2, double>::Zero(),
                      math::Size<2, double> aDimension = math::Size<2, double>{1., 1.}) :
        position{std::move(aPosition)},
        dimension{std::move(aDimension)}
    {}

    math::Position<2, double> position;
    math::Size<2, double> dimension;
};


} // namespace ad
