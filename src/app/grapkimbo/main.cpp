#include "Input.h"
#include "Logging.h"

#include "commons.h"
#include "Configuration.h"

#include <DebugGui/SceneGui.h>

#include "Scenery/RopeGame.h"
#include "Scenery/SplashScene.h"
#include "Scenery/StateMachine.h"

#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <renderer/Image.h>

#include <math/Interpolation.h>

#include <iostream>


using namespace ad;
using namespace ad::grapito;



std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution)
{
    std::shared_ptr<SplashScene> scene = std::make_unique<SplashScene>(aResolution);
    scene->splashes.push_back(
        {
            graphics::Image{"c:/splash_oc.bmp"},
            splash::gDuration,
        });
    scene->splashes.push_back(
        {
            graphics::Image{"c:/cpp.png"},
            splash::gDuration,
            [interpolation = math::makeInterpolation<math::ease::SmoothStep>(math::hdr::gBlack,
                                                                             game::gClearColor,
                                                                             splash::gDuration)]
                (auto aDelta) mutable
                {
                    return interpolation.advance(aDelta);
                },
        });
    return scene;
}


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
        // used for window proportions
        topLevelFlow.pushState(setupSplashScreen(application.getAppInterface()->getWindowSize())); 

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
