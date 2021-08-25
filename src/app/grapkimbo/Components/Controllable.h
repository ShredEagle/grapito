#pragma once

#include "../Input.h"

#include <aunteater/Component.h>

namespace ad {


struct Controllable : public aunteater::Component<Controllable>
{
    explicit Controllable(Controller aController) :
        controller{aController}
    {}

    Controller controller;
};


} // namespace ad
