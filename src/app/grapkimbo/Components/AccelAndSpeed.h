#pragma once

#include "commons.h"
#include <aunteater/Component.h>
#include <math/Vector.h>

#include <vector>

namespace ad {
namespace grapito
{

struct AccelAndSpeed : public aunteater::Component<AccelAndSpeed>
{
    explicit AccelAndSpeed(Vec2 baseSpeed = Vec2::Zero(), float aW = 0.f) :
        accel{Vec2::Zero()},
        speed{std::move(baseSpeed)},
        oldSpeed{std::move(baseSpeed)},
        w{aW}
    {}

    Vec2 accel;
    Vec2 speed;
    Vec2 oldSpeed;
    float w;
};


} // namespace grapito
} // namespace ad
