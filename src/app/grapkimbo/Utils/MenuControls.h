#pragma once


#include "../Input.h"


namespace ad {
namespace grapito {


static const std::initializer_list<Controller> gMenuControllers = {
    Controller::KeyboardMouse,
    Controller::Gamepad_0,
    Controller::Gamepad_1,
    Controller::Gamepad_2,
    Controller::Gamepad_3
}; 


inline Controller isPositiveEdge(const GameInputState & aInputs, Command aButton, Command aAxis, AxisSign aSign)
{
    for (auto controller : gMenuControllers)
    {
        if (aInputs.asButton(controller, aButton, aAxis, aSign, controller::gDeadzone) == PositiveEdge)
        {
            return controller;
        }
    }
    return Controller::_End;
};


inline Controller isPositiveEdge(const GameInputState & aInputs, Command aButton)
{
    for (auto controller : gMenuControllers)
    {
        if (aInputs.get(controller)[aButton].positiveEdge())
        {
            return controller;
        }
    }
    return Controller::_End;
};


} // namespace grapito
} // namespace ad
