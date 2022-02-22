#include "SimpleCollisionTest.h"

#include "Entities.h"
#include "Systems/Physics.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "Timer.h"

#include "TestScenes/SceneChanger.h"

#include <Systems/RenderWorld.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include "Systems/AccelSolver.h"
#include <Utils/DrawDebugStuff.h>

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Controllable.h>
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

#include "Context/Context.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createSimpleCollisionTest(
        float widthA,
        float widthB,
        float baseHeight,
        int qty,
        int index,
        float w,
        aunteater::EntityManager & mEntityManager
        )
{
    for (int i = 0; i < qty; ++i)
    {
        mEntityManager.addEntity(
                aunteater::Entity()
                .add<Position>(Position2{index * 4.f - widthA / 2.f, baseHeight + i * 2.1f}, math::Size<2, float>{widthA, 2.f})
                .add<Body>(
                    math::Rectangle<float>{{0.f, 0.f}, {widthA, 2.f}},
                    BodyType_Dynamic,
                    ShapeType_Hull,
                    CollisionType_Moving_Env,
                    1.,
                    0.,
                    .2
                )
                .add<VisualRectangle>(math::sdr::gCyan)
                .add<AccelAndSpeed>(Vec2{0.f, 0.f}, w)
                );
    }

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{index * 4.f - widthB / 2.f, 2.f}, math::Size<2, float>{widthB, 1.f})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {widthB, 1.f}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.,
                0.,
                .7
            ));
}

SimpleCollisionTest::SimpleCollisionTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>(std::make_shared<Context>("", true, true));
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<RenderWorld>(aApplication.getAppInterface());  

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    createSimpleCollisionTest(2.f, 6.f, 20.f, 1, 1, 0.f, mEntityManager);
    createSimpleCollisionTest(2.f, 0.5f, 4.f, 2, 2, 0.f, mEntityManager);
    createSimpleCollisionTest(2.f, 0.5f, 4.f, 4, 3, 0.f, mEntityManager);
}

bool SimpleCollisionTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
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

} // namespace grapito
} // namespace ad
