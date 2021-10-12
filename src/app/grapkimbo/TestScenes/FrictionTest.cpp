#include "FrictionTest.h"

#include "Entities.h"
#include "Systems/Physics.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"

#include "TestScenes/SceneChanger.h"

#include <Systems/Render.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include "Systems/AccelSolver.h"
#include <Utils/DrawDebugStuff.h>

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Controllable.h>
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createFrictionTest(double height, double friction, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{3., height}, math::Size<2, double>{2., 2.})
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Static_Env,
                1.,
                0.,
                friction
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>(Vec2{2.0, 0.}, 0.)
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{1., height - 1.1}, math::Size<2, double>{12., 1.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {12., 1.}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.,
                0.,
                friction
            ));
}

FrictionTest::FrictionTest(ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    createFrictionTest(5., .2, mEntityManager);
    createFrictionTest(0., .75, mEntityManager);
    createFrictionTest(-5., 1., mEntityManager);
}

bool FrictionTest::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GameInputState> timings;
    InputState pauseInput = aInputState.get(Controller::Keyboard)[Command::Pause];
    InputState step = aInputState.get(Controller::Keyboard)[Command::Step];

    if (pauseInput.positiveEdge())
    {
        pause = !pause;
    }

    if (!pause || step.positiveEdge())
    {
        mSystemManager.pause(false);
        mSystemManager.update(aTimer, aInputState, timings);
        mUI.broadcast(timings);
    }
    else 
    {
        mSystemManager.pause(true);
    }
    return ! mSystemManager.isPaused();
}

} // namespace grapkimbo
} // namespace ad
