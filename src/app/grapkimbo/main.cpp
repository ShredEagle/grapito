#include "build_info.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "Logging.h"
#include "Timer.h"
#include "TopLevelStates.h"

#include "Context/Context.h"

#include "DebugGui/SceneGui.h"

#include "Scenery/RopeGame.h"
#include "Scenery/StateMachine.h"

#include "Utils/DrawDebugStuff.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <platform/Locale.h>

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
        ("skip_splash", po::bool_switch(), "Skip splash screens.")
        ("language", po::value<std::string>(), "Override auto-detected language.")
        ("windowed", po::bool_switch(), "Launch application windowed instead of fullscreen.")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
    }

    return vm;
}      


std::shared_ptr<Context> initializeContext(const po::variables_map & aArguments)
{
    std::shared_ptr<Context> context = 
#if defined(EN_MODE_LAFF)
            std::make_shared<Context>(filesystem::path{"assets/"});
#else
            std::make_shared<Context>(gRepositoryRoot / filesystem::path{"../grapito_media/assets/"});
#endif
        // language
        std::string language;
        if (aArguments.count("language")) 
        {
            language = aArguments["language"].as<std::string>();
        }
        else
        {
            language = platform::getSystemLanguage();
        }
        context->locale.setLanguage(context->locale.isSupported(language) ? language : game::gFallbackLanguage);

    return context;
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

        auto applicationFlags = ad::graphics::ApplicationFlag::Window_Keep_Ratio;
        if (!arguments["windowed"].as<bool>())
        {
            applicationFlags |= ad::graphics::ApplicationFlag::Fullscreen;
        }

        ad::graphics::ApplicationGlfw application(
            "grapkimbo", 
            game::gAppResolution.width(), game::gAppResolution.height(),
            applicationFlags);
        // Need to wait for the graphics logger initialized by ApplicationGlfw constructor.
        initializeLogging();

        setupImGui(application);

        ad::debugDrawer = std::make_unique<ad::debug::DrawDebugStuff>(application);

        ad::grapito::ImguiState imguiState;
        DebugUI debugUI;

        std::shared_ptr<Context> context = initializeContext(arguments);

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

        constexpr float gMaxStepDuration = 1.f/60;
        GrapitoTimer timer{static_cast<float>(glfwGetTime()), gMaxStepDuration};
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
