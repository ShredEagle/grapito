#include "build_info.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "Logging.h"
#include "TopLevelStates.h"

#include "DebugGui/SceneGui.h"

#include "Scenery/RopeGame.h"
#include "Scenery/StateMachine.h"

#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <renderer/Image.h>

#include <resource/ResourceManager.h>

#include <iostream>


using namespace ad;
using namespace ad::grapito;


int main(int /*argc*/, const char ** /*argv*/)
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

        std::shared_ptr<resource::ResourceManager> resources =
            std::make_shared<resource::ResourceManager>(gRepositoryRoot / filesystem::path{"../grapito_media/assets/"});

        // The splashscreens are the initial state
        // note: coordinates are used for window proportions
        StateMachine topLevelFlow{setupSplashScreen(application.getAppInterface()->getWindowSize(),
                                                    *resources)};

        // The next state in the stack is the main menu
        topLevelFlow.putNext(setupMainMenu(resources, application.getAppInterface()));

        GrapitoTimer timer{static_cast<float>(glfwGetTime())};
        GameInputState inputState;

        while(application.handleEvents())
        {
            debugDrawer->clear();
            inputState.readAll(application);
            timer.mark((float)glfwGetTime());
            if (topLevelFlow.update(timer, inputState) == UpdateStatus::SwapBuffers)
            {
                drawImGui(application, debugUI, imguiState);
                renderImGui();
                debugDrawer->render();
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
