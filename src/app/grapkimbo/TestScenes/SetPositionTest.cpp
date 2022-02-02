#include "SetPositionTest.h"

#include "Entities.h"
#include "Systems/Physics.h"
#include "commons.h"
#include "Configuration.h"
#include "Input.h"
#include "../Timer.h"

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

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {

namespace grapito {

void createSetPositionTest(Position2 pos, float angle, Position2 localPoint, Position2 worldPoint, aunteater::EntityManager & mEntityManager)
{

    aunteater::weak_entity bodyB = mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(pos, math::Size<2, float>{3.f, 1.f})
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, {3.f, 1.f}},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Floor,
                1.,
                angle,
                0.
            )
            .add<VisualRectangle>(math::sdr::gMagenta)
            );

    setLocalPointToWorldPos(bodyB, localPoint, worldPoint);
}

SetPositionTest::SetPositionTest(graphics::ApplicationGlfw & aApplication, DebugUI & aUI) :
    mUI{aUI}
{
    mSystemManager.add<RenderWorld>(aApplication.getAppInterface());  

    mEntityManager.addEntity(makeCamera({10.f, 2.f}));

    createSetPositionTest({10.f, 3.f}, 0.f, {0.f,0.f}, {3.f, 0.f},  mEntityManager);
    createSetPositionTest({10.f, 3.f}, 2.f, {0.f, 0.f}, {6.f, 3.f}, mEntityManager);
    createSetPositionTest({10.f, 3.f}, 2.f, {3.f, 0.f}, {3.f, 3.f}, mEntityManager);
    createSetPositionTest({10.f, 3.f}, 8.f, {1.5f, 0.5f}, {12.f, 0.f}, mEntityManager);

}

bool SetPositionTest::update(const GrapitoTimer & aTimer, const GameInputState & aInputState)
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
