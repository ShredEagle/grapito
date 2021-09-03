#include "Game.h"
#include "Game_pendulum.h"
#include "Input.h"

#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

using namespace ad::grapito;


int main(int argc, const char * argv[])
{
    try
    {
        GameInputState inputState;

        ad::Application application("grapkimbo", 1600, 900,
                                ad::Application::Window_Keep_Ratio);
        aunteater::Timer timer{glfwGetTime()};

        //
        // "Game" selection
        // 
        //Game game{application};
        Game_pendulum game{application};

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

