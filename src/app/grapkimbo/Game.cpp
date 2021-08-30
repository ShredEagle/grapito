#include "Game.h"
#include "Components/VisualRectangle.h"
#include "Input.h"

#include <Systems/Render.h>
#include <Systems/ContactConstraintCreation.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include "Systems/AccelSolver.h"
#include "Systems/ImpulseSolver.h"
#include <Utils/DrawDebugStuff.h>

#include "Components/Body.h"
#include <Components/Position.h>
#include <Components/Controllable.h>
#include <Components/AccelAndSpeed.h>

#include <aunteater/UpdateTiming.h>
#include <aunteater/Entity.h>

#include <iostream>

namespace ad {
bool pause = false;

namespace grapkimbo {

Game::Game(Application & aApplication)
{
    debugDrawer = new debug::DrawDebugStuff(aApplication);
    mSystemManager.add<Gravity>();
    mSystemManager.add<Control>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<ContactConstraintCreation>();
    mSystemManager.add<ImpulseSolver>();
    mSystemManager.add<Render>(aApplication); 

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{3., 10.}, math::Size<2, double>{2., 2.})
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                1.
            )
            .add<VisualRectangle>(math::sdr::gCyan, 1.)
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{2., 5.}, math::Size<2, double>{2., 2.})
            .add<Mass>(1.)
            .add<Controllable>(Controller::Keyboard)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.,
                0.
            )
            .add<VisualRectangle>(math::sdr::gCyan, 0.)
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(math::Position<2, double>{2.87, 2.}, math::Size<2, double>{.25, .5})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {.25, .5}},
                BodyType::STATIC,
                ShapeType::HULL
            ));

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{7., 3.}, math::Size<2, double>{2., 2.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.
            )
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<Position>(math::Position<2, double>{7., 10.}, math::Size<2, double>{2., 2.})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::DYNAMIC,
                ShapeType::HULL,
                1.
            )
            .add<AccelAndSpeed>()
            );

    mEntityManager.addEntity(
            aunteater::Entity()
            .add<AccelAndSpeed>()
            .add<Position>(math::Position<2, double>{6., 2.}, math::Size<2, double>{4., .5})
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {4., .5}},
                BodyType::STATIC,
                ShapeType::HULL
            ));
}

bool Game::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
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
