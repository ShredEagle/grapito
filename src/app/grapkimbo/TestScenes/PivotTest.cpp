#include "PivotTest.h"

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

void createPivotTest(double height, aunteater::EntityManager & mEntityManager)
{

    aunteater::weak_entity bodyB = mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{5., height}, math::Size<2, double>{3., 1.})
            .add<PlayerData>(0, math::sdr::gMagenta)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {3., 1.}},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Floor,
                1.,
                0.,
                0.
            )
            .add<VisualRectangle>(math::sdr::gMagenta)
            .add<AccelAndSpeed>()
            );

    aunteater::weak_entity bodyA = mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{-0., height}, math::Size<2, double>{5., 1.})
            .add<PlayerData>(0, math::sdr::gRed)
            .add<VisualRectangle>(math::sdr::gRed)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {5., 1.}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Floor,
                1.,
                0.,
                0.
            ));
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{5., .5},
                Position2{0., .5},
                bodyA,
                bodyB
            ));
}

PivotTest::PivotTest(Application & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    createPivotTest(5., mEntityManager);
}

bool PivotTest::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
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
