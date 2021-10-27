#include "TopLevelStates.h"

#include "Configuration.h"

#include "Scenery/RopeGame.h"

#include <math/Interpolation.h>


namespace ad {
namespace grapito {


std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution)
{
    std::shared_ptr<SplashScene> scene = std::make_unique<SplashScene>(aResolution);
    scene->splashes.push_back(
        {
            graphics::Image{"c:/splash_oc.bmp"},
            splash::gDuration,
        });
    scene->splashes.back().mSpriteOpacity = 
        [interpolation = math::makeInterpolation<math::ease::Linear>(0.f, 1.f, splash::gDuration)]
            (auto aDelta) mutable
            {
                return interpolation.advance(aDelta);
            };

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
            [interpolation = math::makeInterpolation<math::ease::Linear>(1.f, 0.f, splash::gDuration)]
                (auto aDelta) mutable
                {
                    return interpolation.advance(aDelta);
                },
        });
    return scene;
}


std::shared_ptr<MenuScene> setupMainMenu(std::shared_ptr<graphics::AppInterface> & aAppInterface)
{
    return std::make_shared<MenuScene>(
        Menu {
            std::vector<UiButton>{
                { "Start Playing",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.emplaceState<RopeGame>(aAppInterface);
                    }
                },
                { "Drop your Grapple",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aAppInterface->requestCloseApplication();
                    }
                },
            },
        },
        aAppInterface);
}

std::shared_ptr<MenuScene> setupPauseMenu(
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
                { "Restart level",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                        aMachine.emplaceState<RopeGame>(aAppInterface); // new game
                    }
                },
                { "Exit to Main Menu",
                  [](StateMachine & aMachine, std::shared_ptr<graphics::AppInterface> & aAppInterface)
                    {
                        aMachine.popState(); // this
                        aMachine.popState(); // running game
                    }
                },
            },
        },
        aAppInterface,
        std::move(aGameScene)
    );
}



} // namespace grapito
} // namespace ad
