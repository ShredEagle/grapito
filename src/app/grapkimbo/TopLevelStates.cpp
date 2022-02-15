#include "TopLevelStates.h"

#include "Configuration.h"

#include "Scenery/RopeGame.h"

#include "Utils/CompositeTransition.h"

#include <handy/StringId_Interning.h>

#include <math/Interpolation/Interpolation.h>


using namespace ad::handy::literals;


namespace ad {
namespace grapito {


using handy::StringId;
// Interning the strings allow to find them from the hash, if needed (debug).
const StringId menu_multiplayer_sid = handy::internalizeString("menu_multiplayer");
const StringId menu_freesolo_sid    = handy::internalizeString("menu_freesolo");
const StringId menu_floorislava_sid = handy::internalizeString("menu_floorislava");
const StringId menu_exit_sid        = handy::internalizeString("menu_exit");
const StringId menu_resume_sid      = handy::internalizeString("menu_resume");
const StringId menu_restart_sid     = handy::internalizeString("menu_restart");
const StringId menu_main_sid        = handy::internalizeString("menu_main");


std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution,
                                               const Context & aContext)
{
    std::shared_ptr<SplashScene> scene = std::make_unique<SplashScene>(aResolution);
    scene->splashes.push_back(
        {
            arte::ImageRgba{aContext.pathFor("images/splashes/cpp.png"),
                            arte::ImageOrientation::InvertVerticalAxis},
            splash::gDuration,
        });

    CompositeTransition<GLfloat, GrapitoTimer::Value_t> spriteOpacity(0.f);
    spriteOpacity.pushConstant(splash::gHiddenDuration)
                 .pushInterpolation<math::None/*linear*/>(1.f, splash::gLinearDuration)
                 .pushConstant(splash::gConstantDuration)
                 .pushInterpolation<math::None>(0.f, splash::gLinearDuration)
    // Useless: the behaviour is programmed to be "constant at last value anyway
    //             .pushConstant(splash::gHiddenDuration); // T
                 ;
    scene->splashes.back().mSpriteOpacity = 
        [interpolation = spriteOpacity]
            (auto aDelta) mutable
            {
                return interpolation.advance(aDelta);
            };

    scene->splashes.push_back(
        {
            arte::ImageRgba{aContext.pathFor("images/splashes/shred_eagle.png"),
                            arte::ImageOrientation::InvertVerticalAxis},
            splash::gDuration,
            [interpolation = 
                math::makeInterpolation<math::None, math::ease::SmoothStep>(math::hdr::gBlack,
                                                                            game::gClearColor,
                                                                            splash::gDuration)]
                (auto aDelta) mutable
                {
                    return interpolation.advance(aDelta);
                },
            [interpolation = spriteOpacity]
                (auto aDelta) mutable
                {
                    return interpolation.advance(aDelta);
                },
        });
    return scene;
}


std::shared_ptr<MenuScene> setupMainMenu(const std::shared_ptr<Context> & aContext,
                                         std::shared_ptr<graphics::AppInterface> aAppInterface)
{
    return std::make_shared<MenuScene>(
        Menu {
            std::vector<UiButton>{
                { aContext->translate(menu_multiplayer_sid),
                [aContext](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface, const Controller aController)
                    {
                        aMachine.emplaceState<RopeGame>(aContext, aAppInterface, GameMode::Multiplayer, aController);
                    }
                },
                { aContext->translate(menu_freesolo_sid),
                [aContext](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface, const Controller aController)
                    {
                        aMachine.emplaceState<RopeGame>(aContext, aAppInterface, GameMode::Freesolo, aController);
                    }
                },
                { aContext->translate(menu_floorislava_sid),
                [aContext](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface, const Controller aController)
                    {
                        aMachine.emplaceState<RopeGame>(aContext, aAppInterface, GameMode::FloorIsLava, aController);
                    }
                },
                { aContext->translate(menu_exit_sid),
                  [](StateMachine & /*aMachine*/, std::shared_ptr<graphics::AppInterface> & aAppInterface, const Controller)
                    {
                        aAppInterface->requestCloseApplication();
                    }
                },
            },
        },
        aContext->pathFor(menu::gFont),
        std::move(aAppInterface));
}


std::shared_ptr<MenuScene> setupPauseMenu(
    const std::shared_ptr<Context> & aContext,
    std::shared_ptr<graphics::AppInterface> & aAppInterface,
    std::shared_ptr<const GameScene> aGameScene,
    GameMode aGameMode)
{
    return std::make_shared<MenuScene>(
        Menu {
            std::vector<UiButton>{
                { aContext->translate(menu_resume_sid),
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> &, const Controller)
                    {
                        aMachine.popState(); // this
                    }
                },
                { aContext->translate(menu_restart_sid),
                  [aContext, aGameMode](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface, const Controller aController)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                        aMachine.emplaceState<RopeGame>(aContext, aAppInterface, aGameMode, aController); // new game
                    }
                },
                { aContext->translate(menu_main_sid),
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> &, const Controller)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                    }
                },
            },
        },
        aContext->pathFor(menu::gFont),
        aAppInterface,
        std::move(aGameScene)
    );
}



} // namespace grapito
} // namespace ad
