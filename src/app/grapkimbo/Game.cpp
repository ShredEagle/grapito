#include "Game.h"
#include "Input.h"

#include <Systems/Render.h>
#include <Systems/EnvironmentCollision.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include "Systems/AccelSolver.h"
#include "Systems/ImpulseSolver.h"
#include <Utils/DrawDebugStuff.h>

#include <Components/Position.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/Controllable.h>
#include <Components/Mass.h>
#include <Components/AccelAndSpeed.h>

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <stdio.h>

namespace ad {
bool pause = false;

namespace grapkimbo {

Game::Game(Application & aApplication)
{
    debugDrawer = new debug::DrawDebugStuff(aApplication);
    mSystemManager.add<Control>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<EnvironmentCollision>();
    mSystemManager.add<ImpulseSolver>();
    mSystemManager.add<Render>(aApplication); 

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{222., 502.})
            .add<Controllable>(Controller::Keyboard)
            .add<EnvironmentCollisionBox>(
                math::Rectangle<double>{{0., 0.}, {100., 100.}},
                EntityType::PHYSICAL
            )
            .add<AccelAndSpeed>()
            .add<Mass>(1.f)
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{222., 222.})
            .add<EnvironmentCollisionBox>(
                math::Rectangle<double>{{0., 0.}, {100., 100.}},
                EntityType::PHYSICAL
            )
            .add<AccelAndSpeed>()
            .add<Mass>(1.f)
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{200., 200.})
            .add<EnvironmentCollisionBox>(
                math::Rectangle<double>{{0., 0.}, {1000., 20.}},
                EntityType::GROUND
            ));
}

bool Game::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GameInputState> timings;
    /*
    InputState pauseInput = aInputState[COMMAND::PAUSE];
    InputState step = aInputState[COMMAND::STEP_FRAME];
    if (pauseInput.edge && pauseInput.state == GLFW_PRESS)
    {
        pause = !pause;
    }

    if (!pause || (step.edge && step.state == GLFW_PRESS))
    {
        mSystemManager.pause(false);
        */
        mSystemManager.update(aTimer, aInputState, timings);
        /*
    }
    else 
    {
        mSystemManager.pause(true);
    }
*/
    return ! mSystemManager.isPaused();
}

} // namespace grapkimbo
} // namespace ad
