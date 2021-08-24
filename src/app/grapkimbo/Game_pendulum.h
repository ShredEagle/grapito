#pragma once


#include "Input.h"
#include "Systems/Control.h"

#include <aunteater/Engine.h>
#include <aunteater/Timer.h>

#include <engine/Application.h>


namespace ad {
namespace grapkimbo {


class Game_pendulum
{
public:
    Game_pendulum(aunteater::Engine & aEngine, Application & aApplication);

    bool update(const aunteater::Timer & aTimer, GameInputState & aInputState);

private:
    aunteater::Engine & mEntityEngine;
    std::shared_ptr<Control> mControlSystem;
};


} // namespace grapkimbo
} // namespace ad
