#include "FrictionTest.h"

#include "Entities.h"
#include "Systems/Physics.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "../Timer.h"

#include <Systems/RenderWorld.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include <Systems/AccelSolver.h>
#include <Utils/DrawDebugStuff.h>

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/Controllable.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>

#include "Context/Context.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createFrictionTest(float height, float friction, aunteater::EntityManager & mEntityManager)
{
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{3.f, height}, math::Size<2, float>{2.f, 2.f})
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {2.f, 2.f}},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Static_Env,
                1.,
                0.,
                friction
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>(Vec2{2.0f, 0.f}, 0.f)
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{1.f, height - 1.1f}, math::Size<2, float>{12.f, 1.f})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {12.f, 1.f}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.,
                0.,
                friction
            ));
}

FrictionTest::FrictionTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>(std::make_shared<Context>(""));
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<RenderWorld>(aApplication.getAppInterface());  

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    createFrictionTest(5.f, 0.2f, mEntityManager);
    createFrictionTest(0.f, 0.75f, mEntityManager);
    createFrictionTest(-5.f, 1.f, mEntityManager);
}

bool FrictionTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GrapitoTimer, GameInputState> timings;
    InputState pauseInput = aInputState.get(Controller::KeyboardMouse)[Command::Pause];
    InputState step = aInputState.get(Controller::KeyboardMouse)[Command::Step];

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
