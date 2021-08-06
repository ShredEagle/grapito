#pragma once

#include <aunteater/Component.h>

namespace ad {


struct Controllable : public aunteater::Component<Controllable>
{
    //Should have a controller id
    explicit Controllable()
    {}
};


} // namespace ad
