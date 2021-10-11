#include "Input.h"

#include <DebugGui/SceneGui.h>

#include "TestScenes/SceneChanger.h"
#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

using namespace ad::grapito;


int main(int argc, const char * argv[])
{
    try
    {
        GameInputState inputState;
        DebugUI debugUI;

        ad::Application application("grapkimbo", 1600, 900,
                                ad::Application::Window_Keep_Ratio);
        aunteater::Timer timer{glfwGetTime()};

        setupImGui(application);

        ad::debugDrawer = std::make_unique<ad::debug::DrawDebugStuff>(application);
        ad::grapito::ImguiState imguiState;
        //
        // "Game" selection
        // 
        //Game game{application};
        ChangeScene(GameList::GamePendulum, application, debugUI);

        while(application.handleEvents())
        {
            inputState.readAll(application);
            timer.mark(glfwGetTime());
            if (currentGame->update(timer, inputState))
            {
                drawImGui(application, debugUI, imguiState);
                renderImGui();
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

