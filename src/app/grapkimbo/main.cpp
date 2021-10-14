#include "Input.h"
#include "Logging.h"

#include "commons.h"

#include <DebugGui/SceneGui.h>

#include "Scenery/StateMachine.h"
#include "Scenery/RopeGame.h"

#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <iostream>

using namespace ad::grapito;


int main(int argc, const char * argv[])
{
    try
    {
        ad::graphics::ApplicationGlfw application(
            "grapkimbo", 1600, 900,
            ad::graphics::ApplicationGlfw::Window_Keep_Ratio);
        // Need to wait for the graphics logger initialized by ApplicationGlfw constructor.
        initializeLogging();

        setupImGui(application);

        ad::debugDrawer = std::make_unique<ad::debug::DrawDebugStuff>(application);

        ad::grapito::ImguiState imguiState;
        DebugUI debugUI;

        StateMachine topLevelFlow{std::make_shared<RopeGame>(application.getAppInterface())};
        GrapitoTimer timer{static_cast<float>(glfwGetTime())};
        GameInputState inputState;

        while(application.handleEvents())
        {
            inputState.readAll(application);
            timer.mark((float)glfwGetTime());
            if (topLevelFlow.update(timer, inputState) == UpdateStatus::SwapBuffers)
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

