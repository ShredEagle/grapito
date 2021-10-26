#include "WeldTest.h"

#include "../Entities.h"
#include "../commons.h"
#include "../Configuration.h"
#include "../Input.h"

#include "../Systems/Render.h"
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
#include "../Components/WeldJoint.h"

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createWeldTest(float height, aunteater::EntityManager & mEntityManager, float frequencyHertz, float dampingRatio)
{

    aunteater::weak_entity bodyB = mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{5.f, height}, math::Size<2, float>{3.f, 1.f})
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
            .add<Position>(Position2{-0.f, height}, math::Size<2, float>{5.f, 1.f})
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
            .add<WeldJoint>(
                Position2{5.f, 0.5f},
                Position2{0.f, 0.5f},
                frequencyHertz,
                dampingRatio,
                bodyA,
                bodyB
            ));
}

WeldTest::WeldTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();
    mSystemManager.add<Render>(aApplication); 

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    createWeldTest(5., mEntityManager, 15.f, 0.5f);
    createWeldTest(7., mEntityManager, 0.f, 0.f);
}

bool WeldTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
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
