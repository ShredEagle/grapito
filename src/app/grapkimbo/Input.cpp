#include "Input.h"

#include <GLFW/glfw3.h>

#include <cassert>


namespace ad 
{


const KeyboardInputConfig gKeyboardConfig = {
    KeyboardInputMapping{Up,      GLFW_KEY_UP},
    KeyboardInputMapping{Down,    GLFW_KEY_DOWN},
    KeyboardInputMapping{Left,    GLFW_KEY_LEFT},
    KeyboardInputMapping{Right,   GLFW_KEY_RIGHT},
    KeyboardInputMapping{Jump,    GLFW_KEY_SPACE},
    KeyboardInputMapping{Grapple, GLFW_KEY_X},
    KeyboardInputMapping{Pause,   GLFW_KEY_P},
    KeyboardInputMapping{Step,    GLFW_KEY_O},
};


const GamepadInputConfig gGamepadConfig = {
    GamepadInputMapping{Up, GLFW_GAMEPAD_BUTTON_DPAD_UP, Button},
    GamepadInputMapping{Down, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, Button},
    GamepadInputMapping{Left, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, Button},
    GamepadInputMapping{Right, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, Button},
    GamepadInputMapping{Jump, GLFW_GAMEPAD_BUTTON_A, Button},
    GamepadInputMapping{Grapple, GLFW_GAMEPAD_BUTTON_B, Button},
    GamepadInputMapping{ChangeMode, GLFW_GAMEPAD_BUTTON_Y, Button},
    GamepadInputMapping{LeftHorizontalAxis, GLFW_GAMEPAD_AXIS_LEFT_X, Axis},
    GamepadInputMapping{LeftVerticalAxis,   GLFW_GAMEPAD_AXIS_LEFT_Y, AxisInverted},
    GamepadInputMapping{RightHorizontalAxis, GLFW_GAMEPAD_AXIS_RIGHT_X, Axis},
    GamepadInputMapping{RightVerticalAxis,   GLFW_GAMEPAD_AXIS_RIGHT_Y, AxisInverted},
};


void handleButtonEdges(InputState & aInputState, int aGlfwState)
{
    if(aInputState && aGlfwState)
    {
        aInputState.state = Pressed;
    }
    else if(aInputState && ! aGlfwState)
    {
        aInputState.state = NegativeEdge;
    }
    else if(! aInputState && aGlfwState)
    {
        aInputState.state = PositiveEdge;
    }
    else
    {
        aInputState.state = Released;
    }
}


void readKeyboard(const KeyboardInputConfig & aConfig, ControllerInputState & aState, Application & aApplication)
{
    for (size_t i = 0; i < aConfig.size(); ++i)
    {
        handleButtonEdges(aState[aConfig[i].command],
                          aApplication.getKey(aConfig[i].glKeyCode));
    }
}


void readJoystick(int aGlfwJoystickId, const GamepadInputConfig & aConfig, ControllerInputState & aState)
{
    GLFWgamepadstate gamepadState;
    if (glfwGetGamepadState(aGlfwJoystickId, &gamepadState))
    {
        for (GamepadInputConfig::const_iterator mapping = aConfig.begin();
             mapping != aConfig.end();
             ++mapping)
        {
            switch(mapping->nature)
            {
            case Button:
                handleButtonEdges(aState[mapping->command], gamepadState.buttons[mapping->glGamePadCode]);
                break;
            case Axis:
                aState[mapping->command].state = + gamepadState.axes[mapping->glGamePadCode];
                break;
            case AxisInverted:
                aState[mapping->command].state = - gamepadState.axes[mapping->glGamePadCode];
                break;
            }
        }
    }
}


int toGlfwJoystickId(Controller aController)
{
    // Glfw joysticks are zero indexed
    return static_cast<int>(aController) - static_cast<int>(Controller::Gamepad_0);
}


constexpr bool isGamepad(Controller aController)
{
    return aController != Controller::Keyboard;
}


bool isGamepadPresent(Controller aController)
{
    assert(aController >= Controller::Gamepad_0);
    return glfwJoystickIsGamepad(toGlfwJoystickId(aController));
}


void GameInputState::readAll(Application & aApplication)
{
    readKeyboard(gKeyboardConfig,
                 controllerState[static_cast<std::size_t>(Controller::Keyboard)],
                 aApplication);

    for (Controller controller = Controller::Gamepad_0;
         controller != Controller::End;
         controller = static_cast<Controller>(static_cast<std::underlying_type_t<Controller>>(controller) + 1))
    {
        readJoystick(toGlfwJoystickId(controller),
                     gGamepadConfig,
                     controllerState[static_cast<std::size_t>(controller)]);
    }
}


float GameInputState::asAxis(Controller aController,
                             Command aNegativeButton,
                             Command aPositiveButton,
                             Command aGamepadAxis) const
{
    ControllerInputState input = get(aController);
    float axis = 0.;
    if (! isGamepad(aController))
    {
        if (input[aNegativeButton])
        {
            axis -= 1.f;
        }
        if (input[aPositiveButton])
        {
            axis += 1.f;
        }
    }
    else
    {
        axis = input[aGamepadAxis];
    }
    return axis;
}


template <class T>
T filterDeadzoneSingleAxis(T aValue, T aDeadzone, T aDefault = 0)
{
    return std::abs(aValue) <= aDeadzone ? aDefault : aValue;
}


math::Vec<2, float> GameInputState::asDirection(Controller aController,
                                                Command aHorizontalAxis,
                                                Command aVerticalAxis,
                                                float aDeadZone) const
{
    if (! isGamepad(aController))
    {
        return math::Vec<2, float>::Zero();
    }

    ControllerInputState input = get(aController);
    math::Vec<2, float> candidate{input[aHorizontalAxis], input[aVerticalAxis]};
    return candidate.getNorm() > aDeadZone ? candidate : math::Vec<2, float>::Zero();
}

} // namespace ad
