#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"

#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>
#include <aunteater/Timer.h>

#include <engine/Application.h>


namespace ad {
namespace grapito {

class Game_pendulum : public Game
{
public:
    Game_pendulum(Application & aApplication, DebugUI & aUI);
    ~Game_pendulum() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
} // namespace grapito
} // namespace ad
