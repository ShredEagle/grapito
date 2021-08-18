#pragma once

#include <aunteater/Component.h>

namespace ad {


struct Mass : public aunteater::Component<Mass>
{
    explicit Mass(float aMass = 1) :
        mass{aMass}
    {}

    float mass;
};


} // namespace ad
