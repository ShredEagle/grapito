#pragma once

#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Timer.h>
#include <aunteater/SystemManager.h>

#include <engine/Application.h>

#include <iostream>

namespace ad {
namespace grapkimbo {

class Game
{
public:
    Game(Application & aApplication);

    bool update(const aunteater::Timer & aTimer, const GameInputState & aInputState);

private:
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GameInputState> mSystemManager{mEntityManager};
};
}} // namespace ad::grapkimbo
