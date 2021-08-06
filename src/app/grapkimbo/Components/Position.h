#pragma once

#include <aunteater/Component.h>
#include <math/Vector.h>

namespace ad {

struct Position : public aunteater::Component<Position>
{
    explicit Position(math::Position<2, int> aPosition = math::Position<2, int>::Zero()) :
        position{std::move(aPosition)}
    {}

    math::Position<2, int> position;
};


} // namespace ad
