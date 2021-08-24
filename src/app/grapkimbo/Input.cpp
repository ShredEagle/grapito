#include "Input.h"

#include <GLFW/glfw3.h>


namespace ad 
{


const KeyboardInputConfig gKeyboardConfig = {
    KeyboardInputMapping{Up, GLFW_KEY_UP},
    KeyboardInputMapping{Down, GLFW_KEY_DOWN},
    KeyboardInputMapping{Left, GLFW_KEY_LEFT},
    KeyboardInputMapping{Right, GLFW_KEY_RIGHT},
    KeyboardInputMapping{A, GLFW_KEY_X},
    KeyboardInputMapping{B, GLFW_KEY_SPACE},
};


const GamepadInputConfig gGamepadConfig = {
    GamepadInputMapping{Up, GLFW_GAMEPAD_BUTTON_DPAD_UP, Button},
    GamepadInputMapping{Down, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, Button},
    GamepadInputMapping{Left, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, Button},
    GamepadInputMapping{Right, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, Button},
    GamepadInputMapping{A, GLFW_GAMEPAD_BUTTON_A, Button},
    GamepadInputMapping{B, GLFW_GAMEPAD_BUTTON_B, Button},
};



void readKeyboard(const KeyboardInputConfig & aConfig, ControllerInputState & aState, Application & aApplication)
{
    for (size_t i = 0; i < aConfig.size(); ++i)
    {
        //Get keyboard state
        aState[aConfig[i].command] = {
            glfwGetKey(aApplication.getWindow(), aConfig[i].glKeyCode)
        };
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
            if (mapping->nature == Button)
            {
                aState[mapping->command].state = gamepadState.buttons[mapping->glGamePadCode];
            }
            else
            {
                aState[mapping->command].state = gamepadState.axes[mapping->glGamePadCode];
            }
        }
    }
}


int toGlfwJoystickId(Controller aController)
{
    // Glfw joysticks are zero indexed
    return static_cast<int>(aController) - static_cast<int>(Controller::Gamepad_0);
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


} // namespace ad