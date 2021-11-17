#include "TopLevelStates.h"

#include "Configuration.h"

#include "Scenery/RopeGame.h"

#include "Utils/CompositeTransition.h"

#include <math/Interpolation.h>


namespace ad {
namespace grapito {


std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution,
                                               const resource::ResourceManager & aResources)
{
    std::shared_ptr<SplashScene> scene = std::make_unique<SplashScene>(aResolution);
    scene->splashes.push_back(
        {
            graphics::Image{aResources.pathFor("images/splashes/splash.bmp").string()},
            splash::gDuration,
        });

    CompositeTransition<GLfloat, GrapitoTimer::Value_t> spriteOpacity(0.f);
    spriteOpacity.pushConstant(splash::gHiddenDuration)
                 .pushInterpolation<math::ease::Linear>(1.f, splash::gLinearDuration)
                 .pushConstant(splash::gConstantDuration)
                 .pushInterpolation<math::ease::Linear>(0.f, splash::gLinearDuration)
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
            graphics::Image{aResources.pathFor("images/splashes/cpp.png").string()},
            splash::gDuration,
            [interpolation = math::makeInterpolation<math::ease::SmoothStep>(math::hdr::gBlack,
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


std::shared_ptr<MenuScene> setupMainMenu(const std::shared_ptr<resource::ResourceManager> & aResources,
                                         std::shared_ptr<graphics::AppInterface> aAppInterface)
{
    return std::make_shared<MenuScene>(
        Menu {
            std::vector<UiButton>{
                { "Start",
                  [aResources](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.emplaceState<RopeGame>(aResources, aAppInterface);
                    }
                },
                { "Drop Grapple",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aAppInterface->requestCloseApplication();
                    }
                },
            },
        },
        aResources->pathFor(menu::gFont),
        std::move(aAppInterface));
}


std::shared_ptr<MenuScene> setupPauseMenu(
    const std::shared_ptr<resource::ResourceManager> & aResources,
    std::shared_ptr<graphics::AppInterface> & aAppInterface,
    std::shared_ptr<GameScene> aGameScene)
{
    return std::make_shared<MenuScene>(
        Menu {
            std::vector<UiButton>{
                { "Resume",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> &)
                    {
                        aMachine.popState(); // this
                    }
                },
                { "Restart",
                  [aResources](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                        aMachine.emplaceState<RopeGame>(aResources, aAppInterface); // new game
                    }
                },
                { "Main Menu",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                    }
                },
            },
        },
        aResources->pathFor(menu::gFont),
        aAppInterface,
        std::move(aGameScene)
    );
}



} // namespace grapito
} // namespace ad
