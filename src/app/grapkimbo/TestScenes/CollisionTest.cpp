#include "CollisionTest.h"

#include "Entities.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"

#include <Systems/Render.h>
#include <Systems/Physics.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include <Systems/AccelSolver.h>
#include <Utils/DrawDebugStuff.h>

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/Controllable.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <cmath>
#include <iostream>

namespace ad {

namespace grapito {

void createStaticPlatform(Position2 pos, math::Size<2, float> size, float angle, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(pos, size)
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, size},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Static_Env,
                0.,
                angle,
                .9
            ));
}

void createBox(Position2 pos, math::Size<2, float> size, float angularSpeed, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(pos, size)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                1.,
                1.,
                .5
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>(Vec2{0.f, 0.f}, angularSpeed)
            );
}

// This test is basically succesful given our current
// contact solving. It cannot converge without contact persistence.
CollisionTest::CollisionTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<Render>(aApplication); 

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    createStaticPlatform({-2.f, 0.f}, {15.f, 2.f}, -math::pi<float> / 3.f, mEntityManager);
    createStaticPlatform({6.f, 0.f}, {15.f, 2.f}, -2.f * math::pi<float> / 3.f, mEntityManager);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 50; ++j)
        {
            createBox({5.f + i * 1.5f, 5.f + j * 1.5f}, {0.5f, 0.5f}, (j + i * 5) % 3, mEntityManager);
        }
    }
}

bool CollisionTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GrapitoTimer, GameInputState> timings;
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
