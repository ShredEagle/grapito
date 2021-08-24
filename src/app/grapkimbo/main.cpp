#include "Game.h"
#include "Game_pendulum.h"
#include "Input.h"

#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

using namespace ad;


void readJoystick(const GameInputConfig & aConfig, GameInputState & aState)
{
    GLFWgamepadstate gpState;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &gpState))
    {
        for (size_t i = 0; i < aConfig.size(); ++i)
        {
            Input input = aConfig[i];
            aState[i].command = input.command;
            aState[i].nature = input.gamepadNature;
            if (input.isButton())
            {
                aState[i].state = gpState.buttons[input.glGamePadCode];
            }
            else
            {
                aState[i].state = gpState.axes[input.glGamePadCode];
            }
        }
    }
}

int main(int argc, const char * argv[])
{
    try
    {

        GameInputConfig inputConfig = {
            Input{Up, GLFW_KEY_UP, GLFW_GAMEPAD_BUTTON_DPAD_UP, Button},
            Input{Down, GLFW_KEY_DOWN, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, Button},
            Input{Left, GLFW_KEY_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, Button},
            Input{Right, GLFW_KEY_RIGHT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, Button},
            Input{A, GLFW_KEY_X, GLFW_GAMEPAD_BUTTON_A, Button},
            Input{B, GLFW_KEY_SPACE, GLFW_GAMEPAD_BUTTON_B, Button},
        };

        GameInputState inputState;

        Application application("grapkimbo", 1600, 900,
                                Application::Window_Keep_Ratio);
        aunteater::Engine gameEngine;
        aunteater::Timer timer{glfwGetTime()};

        glfwSetInputMode(application.getWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);

        //grapkimbo::Game game{gameEngine, application};
        grapkimbo::Game_pendulum game{gameEngine, application};

        while(application.handleEvents())
        {
            for (size_t i = 0; i < inputConfig.size(); ++i)
            {
                //Get keyboard state
                inputState[i] = {
                    inputConfig[i].command,
                    Button,
                    glfwGetKey(application.getWindow(), inputConfig[i].glKeyCode)
                };
            }
            readJoystick(inputConfig, inputState);

            timer.mark(glfwGetTime());
            if (game.update(timer, inputState))
            {
                application.swapBuffers();
            }

        }
    }
    catch(const std::exception & e)
    {
        std::cerr << "Exception:\n"
                  << e.what()
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_SUCCESS);
}

