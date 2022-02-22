#pragma once

#include "DebugUI.h"
#include "Input.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <graphics/ApplicationGlfw.h>

#include <iostream>
#include <string>

namespace ad {
namespace grapito {

class Game
{
public:
    virtual ~Game() {};
    virtual bool update(const GrapitoTimer & aTimer, const GameInputState & aInputState) = 0;

protected:
    bool pause = false;
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GrapitoTimer, GameInputState> mSystemManager{mEntityManager};
};
}} // namespace ad::grapkimbo
