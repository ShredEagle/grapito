#include "Input.h"

#include "Logging.h"

#include <GLFW/glfw3.h>

#include <cassert>


namespace ad
{
namespace grapito
{


const KeyboardInputConfig gKeyboardConfig = {
    KeyboardInputMapping{Up,        GLFW_KEY_UP},
    KeyboardInputMapping{Down,      GLFW_KEY_DOWN},
    KeyboardInputMapping{Left,      GLFW_KEY_LEFT},
    KeyboardInputMapping{Right,     GLFW_KEY_RIGHT},
    KeyboardInputMapping{Jump,      GLFW_KEY_SPACE},
    KeyboardInputMapping{Grapple,   GLFW_KEY_X},
    KeyboardInputMapping{Start,     GLFW_KEY_ENTER},
    KeyboardInputMapping{Back,      GLFW_KEY_BACKSPACE},

    // Debugging
    KeyboardInputMapping{Pause, GLFW_KEY_P},
    KeyboardInputMapping{Step,  GLFW_KEY_O},
};


const GamepadInputConfig gGamepadConfig = {
    GamepadInputMapping{Up,         GLFW_GAMEPAD_BUTTON_DPAD_UP, Button},
    GamepadInputMapping{Down,       GLFW_GAMEPAD_BUTTON_DPAD_DOWN, Button},
    GamepadInputMapping{Left,       GLFW_GAMEPAD_BUTTON_DPAD_LEFT, Button},
    GamepadInputMapping{Right,      GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, Button},
    GamepadInputMapping{Jump,       GLFW_GAMEPAD_BUTTON_A, Button},
    GamepadInputMapping{Grapple,    GLFW_GAMEPAD_BUTTON_B, Button},
    GamepadInputMapping{ChangeMode, GLFW_GAMEPAD_BUTTON_Y, Button},
    GamepadInputMapping{R3,         GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, Button},
    GamepadInputMapping{Start,      GLFW_GAMEPAD_BUTTON_START, Button},
    GamepadInputMapping{Back,       GLFW_GAMEPAD_BUTTON_BACK, Button},
    GamepadInputMapping{LeftHorizontalAxis,  GLFW_GAMEPAD_AXIS_LEFT_X, Axis},
    GamepadInputMapping{LeftVerticalAxis,    GLFW_GAMEPAD_AXIS_LEFT_Y, AxisInverted},
    GamepadInputMapping{RightHorizontalAxis, GLFW_GAMEPAD_AXIS_RIGHT_X, Axis},
    GamepadInputMapping{RightVerticalAxis,   GLFW_GAMEPAD_AXIS_RIGHT_Y, AxisInverted},
};


void handleButtonEdges(InputState & aInputState, bool aCurrentlyPressed)
{
    if(aInputState && aCurrentlyPressed)
    {
        aInputState.state = Pressed;
    }
    else if(aInputState && ! aCurrentlyPressed)
    {
        aInputState.state = NegativeEdge;
    }
    else if(! aInputState && aCurrentlyPressed)
    {
        aInputState.state = PositiveEdge;
    }
    else
    {
        aInputState.state = Released;
    }
}


void readKeyboard(const KeyboardInputConfig & aConfig, ControllerInputState & aState, graphics::ApplicationGlfw & aApplication)
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
                aState[mapping->command].axis() = + gamepadState.axes[mapping->glGamePadCode];
                break;
            case AxisInverted:
                aState[mapping->command].axis() = - gamepadState.axes[mapping->glGamePadCode];
                break;
            }
        }
    }
}

void readMouse(ControllerInputState & aState, graphics::ApplicationGlfw & aApplication)
{
    float xPos, yPos;
    aApplication.getMousePos(xPos, yPos);

    // Note Ad 2022/01/05 I don't think the mouse is used, and it is not completely implemented
    // (e.g. missing from configurations)
    aState[MouseXPos].axis() = xPos;
    aState[MouseYPos].axis() = yPos;
}


int toGlfwJoystickId(Controller aController)
{
    // Glfw joysticks are zero indexed
    return static_cast<int>(aController) - static_cast<int>(Controller::Gamepad_0);
}


constexpr bool isGamepad(Controller aController)
{
    return aController != Controller::KeyboardMouse;
}


bool isGamepadPresent(Controller aController)
{
    assert(aController >= Controller::Gamepad_0);
    return glfwJoystickIsGamepad(toGlfwJoystickId(aController));
}


GameInputState::GameInputState()
{
    for (const Controller controller : gAllControllers)
    {
        // Gamepads
        if(isGamepad(controller))
        {
            for (const GamepadInputMapping & mapping : gGamepadConfig)
            {
                if (mapping.nature == Axis || mapping.nature == AxisInverted)
                {
                    controllerState[(size_t)controller][mapping.command].state = AxisStatus{};
                }
            }
        }

        // Keyboard is all buttons, the default alternative
        else
        {
            controllerState[(size_t)controller][MouseXPos].state = AxisStatus{};
            controllerState[(size_t)controller][MouseYPos].state = AxisStatus{};
        }
    }
}


void GameInputState::readAll(graphics::ApplicationGlfw & aApplication)
{
    readKeyboard(gKeyboardConfig,
                 controllerState[static_cast<std::size_t>(Controller::KeyboardMouse)],
                 aApplication);
    readMouse(controllerState[static_cast<size_t>(Controller::KeyboardMouse)], aApplication);

    for (Controller controller = Controller::Gamepad_0;
         controller != Controller::_End;
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


ButtonStatus GameInputState::asButton(Controller aController,
                                      Command aButton,
                                      Command aGamepadAxis,
                                      AxisSign aSign,
                                      float aDeadZone) const
{
    assert(aDeadZone >= 0.f);

    ControllerInputState input = get(aController);
    if (isGamepad(aController))
    {
        auto isPressed = [aSign, aDeadZone](float aValue) -> bool
        {
            switch(aSign)
            {
                case AxisSign::Negative:
                    return aValue < -aDeadZone;
                // For invalid values, consider it positive case the default
                case AxisSign::Positive:
                default:
                    return aValue > aDeadZone;
            }
        };

        // Note: this will not really store the previous status, but only whether it was pressed or not.
        // i.e. it is loosing the edge, but this should have no impact.
        InputState inState{isPressed(input[aGamepadAxis].axis().previous) ? Pressed : Released};
        handleButtonEdges(inState, isPressed(input[aGamepadAxis].axis().current));
        return inState;
    }
    else
    {
        return input[aButton];
    }
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

math::Vec<2, float> GameInputState::asDirection(Controller aController,
    Command aHorizontalAxis,
    Command aVerticalAxis,
    float aHorizontalDeadZone,
    float aVerticalDeadZone) const
{
    if (!isGamepad(aController))
    {
        return math::Vec<2, float>::Zero();
    }

    ControllerInputState input = get(aController);
    math::Vec<2, float> candidate{ input[aHorizontalAxis], input[aVerticalAxis] };
    candidate.x() = std::abs(candidate.x()) > aHorizontalDeadZone ? candidate.x() : 0.f;
    candidate.y() = std::abs(candidate.y()) > aVerticalDeadZone ? candidate.y() : 0.f;
    return candidate;
}

} // namespace grapito
} // namespace ad
