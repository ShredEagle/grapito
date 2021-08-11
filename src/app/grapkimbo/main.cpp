#include "Game.h"
#include "Input.h"

#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

using namespace ad;


int main(int argc, const char * argv[])
{
    try
    {

        gameInputConfig inputStore = {
            Input{UP, GLFW_KEY_UP, GLFW_GAMEPAD_AXIS_LEFT_Y},
            Input{DOWN, GLFW_KEY_DOWN, GLFW_GAMEPAD_AXIS_LEFT_Y},
            Input{LEFT, GLFW_KEY_LEFT, GLFW_GAMEPAD_AXIS_LEFT_X},
            Input{RIGHT, GLFW_KEY_RIGHT, GLFW_GAMEPAD_AXIS_LEFT_X},
            Input{A, GLFW_KEY_X, GLFW_GAMEPAD_AXIS_LEFT_Y},
        };

        gameInputState inputState;

        Application application("grapkimbo", 1600, 900,
                                Application::Window_Keep_Ratio);
        aunteater::Engine gameEngine;
        aunteater::Timer timer{glfwGetTime()};

        glfwSetInputMode(application.getWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);

        grapkimbo::Game game{gameEngine, application};

        while(application.handleEvents())
        {
            for (size_t i = 0; i < inputStore.size(); ++i)
            {
                //Get keyboard state
                inputState[i] = {
                    inputStore[i].command,
                    glfwGetKey(application.getWindow(), inputStore[i].GLKeyCode)
                };
            }

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

