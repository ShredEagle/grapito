#pragma once

#include <aunteater/Component.h>
#include <math/Vector.h>

#include <vector>

namespace ad {

struct AccelAndSpeed : public aunteater::Component<AccelAndSpeed>
{
    explicit AccelAndSpeed(math::Vec<2, double> baseSpeed = math::Vec<2, double>::Zero()) :
        accel{math::Vec<2, double>::Zero()},
        speed{std::move(baseSpeed)},
        oldSpeed{std::move(baseSpeed)}
    {}

    math::Vec<2, double> accel;
    math::Vec<2, double> speed;
    math::Vec<2, double> oldSpeed;
    float w;
};


} // namespace ad
