#include "SimpleCollisionTest.h"

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

void createSimpleCollisionTest(double widthA, double widthB, int index, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{index * 4 - widthA / 2, 4.}, math::Size<2, double>{widthA, 2.})
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {widthA, 2.}},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Static_Env,
                1.,
                0.,
                1.
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>(Vec2{0., 0.}, 0.)
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{index * 4 - widthB / 2 , 2.}, math::Size<2, double>{widthB, 1.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {widthB, 1.}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.,
                0.,
                1.
            ));
}

SimpleCollisionTest::SimpleCollisionTest(Application & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    createSimpleCollisionTest(2., .5, 1, mEntityManager);
}

bool SimpleCollisionTest::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
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
