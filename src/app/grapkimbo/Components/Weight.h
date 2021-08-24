#pragma once

#include <aunteater/Component.h>

namespace ad {


struct Weight : public aunteater::Component<Weight>
{
    explicit Weight(double aMass = 1) :
        mass{aMass}
    {}

    double mass;
};


} // namespace ad
