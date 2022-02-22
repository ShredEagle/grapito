#pragma once

#include "../Input.h"

#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

struct Controllable : public aunteater::Component<Controllable>
{
    explicit Controllable(Controller aController) :
        controller{aController}
    {}

    Controller controller;
};


} // namespace grapito
} // namespace ad
