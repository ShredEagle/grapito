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

#include <boost/program_options.hpp>

#include <iostream>


//#define EN_MODE_LAFF

using namespace ad;
using namespace ad::grapito;

namespace po = boost::program_options;

po::variables_map handleCommandLine(int argc, const char ** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message.")
        ("skip_splash", po::bool_switch(), "Skip splash screens.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
    }

    return vm;
}      


int main(int argc, const char ** argv)
{
    try
    {
        po::variables_map arguments = handleCommandLine(argc, argv);

        if (arguments.count("help")) 
        {
            std::exit(EXIT_SUCCESS);
        }

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
#if defined(EN_MODE_LAFF)
            std::make_shared<Context>(filesystem::path{"assets/"});
#else
            std::make_shared<Context>(gRepositoryRoot / filesystem::path{"../grapito_media/assets/"});
#endif
        context->locale.setLanguage("es");

        StateMachine topLevelFlow;
        
        // Important: Here states are added to the top with pushState, so they are added in reverse order.

        // The next state in the stack is the main menu
        topLevelFlow.pushState(setupMainMenu(context, application.getAppInterface()));

        if (!arguments["skip_splash"].as<bool>())
        {
            // note: coordinates are used for window proportions
            topLevelFlow.pushState(setupSplashScreen(application.getAppInterface()->getWindowSize(),
                                                     *context));
        }

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
