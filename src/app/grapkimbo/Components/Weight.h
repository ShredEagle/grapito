#pragma once

#include <aunteater/Component.h>

namespace ad {


struct Weight : public aunteater::Component<Weight>
{
    explicit Weight(float aWeight = 1) :
        weight{aWeight}
    {}

    float weight;
};


} // namespace ad
