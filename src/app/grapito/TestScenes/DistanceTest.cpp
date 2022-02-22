#include "DistanceTest.h"

#include "../Entities.h"
#include "../commons.h"
#include "../Configuration.h"
#include "../Input.h"
#include "../Timer.h"

#include "../Systems/RenderWorld.h"
#include "../Systems/Control.h"
#include "../Systems/Gravity.h"
#include "../Systems/Physics.h"
#include "../Systems/AccelSolver.h"
#include "../Utils/DrawDebugStuff.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/Body.h"
#include "../Components/Controllable.h"
#include "../Components/Position.h"
#include "../Components/VisualRectangle.h"
#include "../Components/DistanceJoint.h"

#include "../Context/Context.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createDistanceTest(
        float height,
        float x,
        aunteater::EntityManager & mEntityManager,
        float frequencyHertz,
        float dampingRatio,
        float aMinRatio = 0.f,
        float aMaxRatio = 0.f,
        float aLength = 2.f
        )
{

    aunteater::weak_entity bodyB = mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{x, height}, math::Size<2, float>{3.f, 1.f})
            .add<PlayerData>(0, math::sdr::gMagenta)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {3.f, 1.f}},
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
            .add<Position>(Position2{x - 5.f, height + 2.f}, math::Size<2, float>{5.f, 1.f})
            .add<PlayerData>(0, math::sdr::gRed)
            .add<VisualRectangle>(math::sdr::gRed)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {5.f, 1.f}},
                BodyType_Static,
                ShapeType_Hull,
                CollisionType_Floor,
                1.,
                0.,
                0.
            ));

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<DistanceJoint>(
                Position2{5.f, 0.5f},
                Position2{0.f, 0.5f},
                frequencyHertz,
                dampingRatio,
                aMinRatio,
                aMaxRatio,
                aLength,
                bodyA,
                bodyB
            ));
}

DistanceTest::DistanceTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>(std::make_shared<Context>("", true, true));
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<RenderWorld>(aApplication.getAppInterface()); 

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    //createDistanceTest(5.f, -5.f, mEntityManager, 15.f, 0.5f);
    //createDistanceTest(10.f, -5.f, mEntityManager, 15.f, 0.5f, 0.f, 1.f);

    //createDistanceTest(5.f, 5.f, mEntityManager, 0.f, 0.f);
    //createDistanceTest(10.f, 5.f, mEntityManager, 15.f, 0.5f, 1.f, 0.f);

    createDistanceTest(10.f, 15.f, mEntityManager, 15.f, 0.5f, 1.f, 0.5f, 5.f);
}

bool DistanceTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
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
