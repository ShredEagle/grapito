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

#include <iostream>

namespace ad {

namespace grapito {

CollisionTest::CollisionTest(Application & aApplication)
{
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<ContactConstraintCreation>();
    mSystemManager.add<ImpulseSolver>();
    mSystemManager.add<Render>(aApplication); 

    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera({10., 2.}));

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{3., 10.}, math::Size<2, double>{2., 2.})
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                1.,
                .5
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{2., 5.}, math::Size<2, double>{2., 2.})
            .add<Mass>(1.)
            .add<Controllable>(Controller::Keyboard)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                0.,
                .5
            )
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{2.87, 2.}, math::Size<2, double>{.25, .5})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {.25, .5}},
                BodyType::STATIC,
                ShapeType::HULL,
                0.,
                0.,
                .5
            ));

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{7., 3.}, math::Size<2, double>{2., 2.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                0.,
                .5
            )
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{7., 10.}, math::Size<2, double>{2., 2.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                0.,
                .5
            )
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(Position2{6., 2.}, math::Size<2, double>{4.3, .5})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {4.3, .5}},
                BodyType::STATIC,
                ShapeType::HULL,
                0.,
                0.,
                .5
            ));

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(Position2{10., 4.}, math::Size<2, double>{2., 2.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                0.,
                .5
            )
            .add<AccelAndSpeed>()
            );
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
        //mUI.broadcast(timings);
    }
    else 
    {
        mSystemManager.pause(true);
    }
    return ! mSystemManager.isPaused();
}

} // namespace grapkimbo
} // namespace ad
