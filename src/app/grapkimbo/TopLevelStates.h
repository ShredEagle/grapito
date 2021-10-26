#pragma once


#include "Scenery/MenuScene.h"
#include "Scenery/SplashScene.h"

#include <graphics/AppInterface.h>

#include <memory> // for shared_ptr


namespace ad {
namespace grapito {


std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution);


std::shared_ptr<MenuScene> setupMainMenu(std::shared_ptr<graphics::AppInterface> & aAppInterface);


std::shared_ptr<MenuScene> setupPauseMenu(std::shared_ptr<graphics::AppInterface> & aAppInterface);


} // namespace grapito
} // namespace ad
