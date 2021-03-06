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

#include "Windows/Console.h"

#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>

#include <platform/Gui.h>
#include <platform/Locale.h>
#include <platform/Path.h>

#include <boost/program_options.hpp>

#include <iostream>


using namespace ad;
using namespace ad::grapito;

namespace po = boost::program_options;


po::variables_map handleCommandLine(int argc, const char ** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message.")
        ("console",     po::bool_switch(),          "Attach a console for output streams.")
        ("language",    po::value<std::string>(),   "Override auto-detected language.")
        ("skip-splash", po::bool_switch(),          "Skip splash screens.")
        ("windowed",    po::bool_switch(),          "Launch application windowed instead of fullscreen.")
        ("nosound",     po::bool_switch(),          "Launch application with no sound (mostly for profiling)")
        ("nobgmusic",   po::bool_switch(),          "Launch application with no background music")
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
#if defined(SHRED_DEV_ASSETS)
            std::make_shared<Context>(gRepositoryRoot
                                      / filesystem::path{"../grapito_media/assets/"}, aArguments["nosound"].as<bool>(), aArguments["nobgmusic"].as<bool>());
#else
            std::make_shared<Context>(platform::getExecutableFilePath().parent_path()
                                      / filesystem::path{"assets/"}, aArguments["nosound"].as<bool>(), aArguments["nobgmusic"].as<bool>());
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
        initializeLogging();
    }
    catch(const std::exception & e)
    {
        platform::showErrorBox(e.what(), "Error " + getVersionedName());
        std::cerr << "Could not initialize logging, due to exception:\n" << e.what();
        std::exit(EXIT_FAILURE);
    }

    try
    {
        ADLOG(info)("Launching {}.", getVersionedName());

        po::variables_map arguments = handleCommandLine(argc, argv);

        if (arguments.count("help"))
        {
            std::exit(EXIT_SUCCESS);
        }

        if (arguments["console"].as<bool>())
        {
            // no-op on non-Windows
            windows::attachOutputToConsole();
        }

        auto applicationFlags = ad::graphics::ApplicationFlag::Window_Keep_Ratio;
        if (!arguments["windowed"].as<bool>())
        {
            applicationFlags |= ad::graphics::ApplicationFlag::Fullscreen;
        }

#if defined(BUILD_RELEASEMODE)
        applicationFlags |= ad::graphics::ApplicationFlag::HideCursor;
#endif

        ad::graphics::ApplicationGlfw application(
            getVersionedName(),
            game::gAppResolution.width(), game::gAppResolution.height(),
            applicationFlags);

#if defined(SHRED_DISPLAY_IMGUI)
        setupImGui(application);
        ad::grapito::ImguiState imguiState;
#endif

        ad::debugDrawer = std::make_unique<ad::debug::DrawDebugStuff>(application);

#if not defined(BUILD_RELEASEMODE)
        DebugUI debugUI;
#endif

        std::shared_ptr<Context> context = initializeContext(arguments);

        StateMachine topLevelFlow;

        // Important: Here states are added to the top with pushState, so they are added in reverse order.

        // The next state in the stack is the main menu
        topLevelFlow.pushState(setupMainMenu(context, application.getAppInterface()));

        if (!arguments["skip-splash"].as<bool>())
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
#if defined(SHRED_DISPLAY_IMGUI)
                drawImGui(application, debugUI, imguiState);
                renderImGui();
#endif
                debugDrawer->render();
                application.swapBuffers();
            }
        }
    }
    catch(const std::exception & e)
    {
        platform::showErrorBox(e.what(), "Error " + getVersionedName());
        ADLOG(critical)("Terminating due to exception:\n{}", e.what());
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_SUCCESS);
}
