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

class PivotTest : public Game
{
public:
    PivotTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI);
    ~PivotTest() = default;

    bool update(const GrapitoTimer & aTimer, const GameInputState & aInputState);
private:
    DebugUI & mUI;
};
}} // namespace ad::grapito
