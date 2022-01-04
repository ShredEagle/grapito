#pragma once


#include "../Input.h"


namespace ad {
namespace grapito {


static const std::initializer_list<Controller> gMenuControllers = {Controller::KeyboardMouse, Controller::Gamepad_0}; 


inline bool isPositiveEdge(const GameInputState & aInputs, Command aButton, Command aAxis, AxisSign aSign)
{
    for (auto controller : gMenuControllers)
    {
        if (aInputs.asButton(controller, aButton, aAxis, aSign, controller::gDeadzone) == PositiveEdge)
        {
            return true;
        }
    }
    return false;
};


inline bool isPositiveEdge(const GameInputState & aInputs, Command aButton)
{
    for (auto controller : gMenuControllers)
    {
        if (aInputs.get(controller)[aButton].positiveEdge())
        {
            return true;
        }
    }
    return false;
};


} // namespace grapito
} // namespace ad
