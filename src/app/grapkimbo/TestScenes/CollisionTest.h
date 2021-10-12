#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <graphics/ApplicationGlfw.h>

#include <iostream>

namespace ad {
namespace grapito {

class CollisionTest : public Game
{
public:
    CollisionTest(ApplicationGlfw & aApplication, DebugUI & aUI);
    ~CollisionTest() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}
}
