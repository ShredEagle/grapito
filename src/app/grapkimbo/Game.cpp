#include "Game.h"
#include "Input.h"

#include <Systems/Render.h>
#include <Systems/EnvironmentCollision.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include <Systems/SpeedResolution.h>

#include <Components/Position.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/Environment.h>
#include <Components/Controllable.h>
#include <Components/Weight.h>
#include <Components/ForceAndSpeed.h>

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
   mEntityEngine.addSystem<EnvironmentCollision>();
   mEntityEngine.addSystem<Gravity>();
   mEntityEngine.addSystem<SpeedResolution>();

   aEngine.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, int>{200, 200})
            .add<Controllable>()
            .add<EnvironmentCollisionBox>(math::Rectangle<int>{{0,0}, {20,20}})
            .add<ForceAndSpeed>()
            .add<Weight>(1)
           );

   aEngine.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, int>{0, 0})
            .add<Environment>()
            .add<EnvironmentCollisionBox>(math::Rectangle<int>{{0,0}, {1000,20}})
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
