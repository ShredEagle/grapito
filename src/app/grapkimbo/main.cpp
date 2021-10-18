#include "Input.h"
#include "Logging.h"

#include "commons.h"

#include <DebugGui/SceneGui.h>

#include "TestScenes/SceneChanger.h"
#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <iostream>

using namespace ad::grapito;

int main(int argc, const char * argv[])
{
    try
    {
        initializeLogging();

        GameInputState inputState;
        DebugUI debugUI;

        ad::graphics::ApplicationGlfw application(
            "grapkimbo", 1600, 900,
            ad::graphics::ApplicationGlfw::Window_Keep_Ratio);
        GrapitoTimer timer{static_cast<float>(glfwGetTime())};

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
            timer.mark((float)glfwGetTime());

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

