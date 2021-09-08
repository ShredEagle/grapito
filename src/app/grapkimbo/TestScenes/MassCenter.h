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

class MassCenterTest : public Game
{
public:
    MassCenterTest(Application & aApplication);

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);
};
}} // namespace ad::grapkimbo
