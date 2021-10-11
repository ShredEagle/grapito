#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <graphics/Application.h>

#include <iostream>

namespace ad {
namespace grapito {

class FrictionTest : public Game
{
public:
    FrictionTest(Application & aApplication, DebugUI & aUI);
    ~FrictionTest() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}
}
