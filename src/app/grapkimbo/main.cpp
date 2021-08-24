#include "Game.h"
#include "Game_pendulum.h"
#include "Input.h"

#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

using namespace ad;


int main(int argc, const char * argv[])
{
    try
    {
        GameInputState inputState;

        Application application("grapkimbo", 1600, 900,
                                Application::Window_Keep_Ratio);
        aunteater::Engine gameEngine;
        aunteater::Timer timer{glfwGetTime()};

        glfwSetInputMode(application.getWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);

        //
        // "Game" selection
        // 
        //grapkimbo::Game game{gameEngine, application};
        grapkimbo::Game_pendulum game{gameEngine, application};

        while(application.handleEvents())
        {
            inputState.readAll(application);
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

