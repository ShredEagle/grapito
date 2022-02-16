#pragma once

#include <aunteater/Component.h>

namespace ad {
namespace grapito
{


struct Mass : public aunteater::Component<Mass>
{
    explicit Mass(double aMass) :
        mass{aMass}
    {}

    double mass;
};


} // namespace grapito
} // namespace ad
