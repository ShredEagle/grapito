#include "SetPositionTest.h"

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

void createSetPositionTest(Position2 pos, double angle, Position2 localPoint, Position2 worldPoint, aunteater::EntityManager & mEntityManager)
{

    aunteater::weak_entity bodyB = mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(pos, math::Size<2, double>{3., 1.})
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {3., 1.}},
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
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    createSetPositionTest({10., 3.}, 0., {0.,0.}, {3., 0.},  mEntityManager);
    createSetPositionTest({10., 3.}, 2., {0., 0.}, {6., 3.}, mEntityManager);
    createSetPositionTest({10., 3.}, 2., {3., 0.}, {3., 3.}, mEntityManager);
    createSetPositionTest({10., 3.}, 8., {1.5, 0.5}, {12., 0.}, mEntityManager);

}

bool SetPositionTest::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
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

        debugDrawer->drawPoint({
            {3., 0.},
            0.07,
            {255,255,255}
        });
        debugDrawer->drawPoint({
            {3., 3.},
            0.07,
            {255,255,255}
        });
        debugDrawer->drawPoint({
            {6., 3.},
            0.07,
            {255,255,255}
        });
        debugDrawer->drawPoint({
            {12., 0.},
            0.07,
            {255,255,255}
        });
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
