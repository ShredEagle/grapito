#include "CollisionTest.h"

#include "Entities.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"

#include "TestScenes/SceneChanger.h"

#include <Systems/Render.h>
#include <Systems/ContactConstraintCreation.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include "Systems/AccelSolver.h"
#include "Systems/ImpulseSolver.h"
#include <Utils/DrawDebugStuff.h>

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Controllable.h>
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <cmath>
#include <iostream>

namespace ad {

namespace grapito {

void createStaticPlatform(Position2 pos, math::Size<2, double> size, double angle, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(pos, size)
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, size},
                BodyType::STATIC,
                ShapeType::HULL,
                0.,
                angle,
                .9
            ));
}

void createBox(Position2 pos, math::Size<2, double> size, double angularSpeed, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(pos, size)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, size},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                1.,
                .5
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>(Vec2{0., 0.}, angularSpeed)
            );
}

// This test is basically succesful given our current
// contact solving. It cannot converge without contact persistence.
CollisionTest::CollisionTest(Application & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<ContactConstraintCreation>();
    mSystemManager.add<ImpulseSolver>();
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    createStaticPlatform({-2., 0.}, {15., 2.}, -M_PI / 3, mEntityManager);
    createStaticPlatform({6., 0.}, {15., 2.}, -2 * M_PI / 3, mEntityManager);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            createBox({5. + i * 1.5, 5. + j * 1.5}, {1., 1.}, (j + i * 5) % 3, mEntityManager);
        }
    }
}

bool CollisionTest::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
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
