#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"

#include "Utils/TimeSystemsUpdater.h"

#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>
#include <aunteater/Timer.h>

#include <graphics/ApplicationGlfw.h>


namespace ad {
namespace grapito {


class Game_pendulum : public Game
{
public:
    Game_pendulum(graphics::ApplicationGlfw & aApplication, DebugUI & aUI);
    ~Game_pendulum() = default;

    bool update(const GrapitoTimer & aTimer, const GameInputState & aInputState);

private:
    DebugUI & mUI;
    TimeSystemsUpdater<GameInputState> mUpdater;
};


} // namespace grapito
} // namespace ad
