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

Game::Game(Application & aApplication)
{
   mSystemManager.add<Render>(aApplication); 
   mSystemManager.add<Control>();
   mSystemManager.add<EnvironmentCollision>();
   mSystemManager.add<Gravity>();
   mSystemManager.add<SpeedResolution>();

   mEntityManager.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, double>{200., 800.})
            .add<Controllable>(Controller::Keyboard)
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {20., 20.}})
            .add<ForceAndSpeed>()
            .add<Weight>(1.f)
           );

   mEntityManager.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, double>{0., 0.})
            .add<Environment>()
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {1000., 20.}})
           );
}

bool Game::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GameInputState> timings;
    mSystemManager.update(aTimer, aInputState, timings);

    return ! mSystemManager.isPaused();
}

} // namespace grapkimbo
} // namespace ad
