#pragma once

#include "DebugUI.h"
#include "Game.h"
#include "Input.h"

#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <graphics/ApplicationGlfw.h>

namespace ad {
namespace grapito {

class SetPositionTest : public Game
{
public:
    SetPositionTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI);
    ~SetPositionTest() = default;

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}} // namespace ad::grapkimbo
