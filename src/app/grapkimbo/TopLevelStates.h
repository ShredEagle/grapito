#pragma once


#include "Scenery/MenuScene.h"
#include "Scenery/SplashScene.h"

#include <graphics/AppInterface.h>

#include <resource/ResourceManager.h>

#include <memory> // for shared_ptr


namespace ad {
namespace grapito {


std::shared_ptr<SplashScene> setupSplashScreen(math::Size<2, int> aResolution,
                                               const resource::ResourceManager & aResources);


std::shared_ptr<MenuScene> setupMainMenu(const std::shared_ptr<resource::ResourceManager> & aResources,
                                         std::shared_ptr<graphics::AppInterface> aAppInterface);
    
    

std::shared_ptr<MenuScene> setupPauseMenu(
    const std::shared_ptr<resource::ResourceManager> & aResources,
    std::shared_ptr<graphics::AppInterface> & aAppInterface,
    std::shared_ptr<GameScene> aGameScene);


} // namespace grapito
} // namespace ad
