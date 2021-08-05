#pragma once

#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/Engine.h>
#include <aunteater/Timer.h>

#include <engine/Application.h>

#include <iostream>

namespace ad {
namespace grapkimbo {

class Game
{
public:
    Game(aunteater::Engine & aEngine, Application & aApplication);

    bool update(const aunteater::Timer & aTimer, gameInputState & aInputState);

private:
    aunteater::Engine & mEntityEngine;
    std::shared_ptr<Control> mControlSystem;
};
}} // namespace ad::grapkimbo
