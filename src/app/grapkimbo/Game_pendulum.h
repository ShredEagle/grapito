#pragma once


#include "DebugUI.h"
#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>
#include <aunteater/Timer.h>

#include <engine/Application.h>


namespace ad {
namespace grapkimbo {

class Game_pendulum
{
public:
    Game_pendulum(Application & aApplication);

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);

private:
    aunteater::EntityManager  mEntityManager;
    aunteater::SystemManager<GameInputState>  mSystemManager{mEntityManager};
    DebugUI mUI;
};


} // namespace grapkimbo
} // namespace ad
