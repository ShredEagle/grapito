#pragma once

#include "DebugUI.h"
#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <engine/Application.h>

#include <iostream>
#include <string>

namespace ad {
namespace grapito {

class Game
{
public:
    virtual ~Game() {};
    virtual bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState) = 0;

protected:
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GameInputState> mSystemManager{mEntityManager};
    //DebugUI mUI;
};
}} // namespace ad::grapkimbo
