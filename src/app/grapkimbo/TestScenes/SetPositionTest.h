#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"

#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <engine/Application.h>

namespace ad {
namespace grapito {

class SetPositionTest : public Game
{
public:
    SetPositionTest(Application & aApplication, DebugUI & aUI);
    ~SetPositionTest() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}} // namespace ad::grapkimbo
