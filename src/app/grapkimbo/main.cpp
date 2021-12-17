#include "build_info.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "Logging.h"
#include "TopLevelStates.h"

#include "Context/Context.h"

#include "DebugGui/SceneGui.h"

#include "Scenery/RopeGame.h"
#include "Scenery/StateMachine.h"

#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <iostream>


using namespace ad;
using namespace ad::grapito;


int main(int /*argc*/, const char ** /*argv*/)
{
    try
    {
        ad::graphics::ApplicationGlfw application(
            "grapkimbo", game::gAppResolution.width(), game::gAppResolution.height(),
            ad::graphics::ApplicationGlfw::Window_Keep_Ratio);
        // Need to wait for the graphics logger initialized by ApplicationGlfw constructor.
        initializeLogging();

        setupImGui(application);

        ad::debugDrawer = std::make_unique<ad::debug::DrawDebugStuff>(application);

        ad::grapito::ImguiState imguiState;
        DebugUI debugUI;

        std::shared_ptr<Context> context = 
            std::make_shared<Context>(gRepositoryRoot / filesystem::path{"../grapito_media/assets/"});
        context->locale.setLanguage("es");

        // The splashscreens are the initial state
        // note: coordinates are used for window proportions
        StateMachine topLevelFlow{setupSplashScreen(application.getAppInterface()->getWindowSize(),
                                                    *context)};

        // The next state in the stack is the main menu
        topLevelFlow.putNext(setupMainMenu(context, application.getAppInterface()));

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
