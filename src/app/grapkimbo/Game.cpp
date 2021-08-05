#include "Game.h"
#include "Input.h"

#include <Systems/Render.h>
#include <Components/Position.h>
#include <Components/Controllable.h>

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <stdio.h>

namespace ad {
namespace grapkimbo {

Game::Game(aunteater::Engine & aEngine, Application & aApplication) :
    mEntityEngine(aEngine)
{
   mEntityEngine.addSystem<Render>(aApplication); 
   mControlSystem = mEntityEngine.addSystem<Control>();
   aEngine.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, int>{100, 100})
            .add<Controllable>()
           );
}

bool Game::update(const aunteater::Timer & aTimer, gameInputState & aInputState)
{
    aunteater::UpdateTiming timings;
    mControlSystem->loadInputState(aInputState);
    mEntityEngine.update(aTimer, timings);

    return ! mEntityEngine.isPaused();
}

} // namespace grapkimbo
} // namespace ad
