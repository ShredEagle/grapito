#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <engine/Application.h>

#include <iostream>

namespace ad {
namespace grapito {

class SimpleCollisionTest : public Game
{
public:
    SimpleCollisionTest(Application & aApplication, DebugUI & aUI);
    ~SimpleCollisionTest() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}
}