#include "Game_pendulum.h"

#include <Components/Controllable.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/ForceAndSpeed.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/Weight.h>

#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include <Systems/PendulumSimulation.h>
#include <Systems/Render.h>
#include <Systems/SpeedResolution.h>

#include <aunteater/Entity.h>
#include <aunteater/UpdateTiming.h>


namespace ad {
namespace grapkimbo {


Game_pendulum::Game_pendulum(aunteater::Engine & aEngine, Application & aApplication) :
    mEntityEngine(aEngine)
{
   mEntityEngine.addSystem<Render>(aApplication); 
   mControlSystem = mEntityEngine.addSystem<Control>();
   mEntityEngine.addSystem<PendulumSimulation>();
   mEntityEngine.addSystem<Gravity>();
   mEntityEngine.addSystem<SpeedResolution>();

   // Anchor point
   aEngine.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, double>{4., 6.}, math::Size<2, double>{2., 2.} )
            //.add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {50., 50.}})
           );

   // Player
   aEngine.addEntity(
           aunteater::Entity()
            .add<Position>(math::Position<2, double>{0., 0.}, math::Size<2, double>{0.8, 1.9}) // The position will be set by pendulum simulation
            //.add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {30., 30.}})
            .add<Pendular>(Pendular{ {5., 6.}, math::Radian<double>{3.14/3.}, 3. })
            .add<Controllable>()
           );
}

bool Game_pendulum::update(const aunteater::Timer & aTimer, gameInputState & aInputState)
{
    aunteater::UpdateTiming timings;
    mControlSystem->loadInputState(aInputState);
    mEntityEngine.update(aTimer, timings);

    return ! mEntityEngine.isPaused();
}

} // namespace grapkimbo
} // namespace ad
