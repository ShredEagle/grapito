#include "Game_pendulum.h"
#include "Components/Body.h"
#include "Player.h"
#include "Systems/AccelSolver.h"

#include <Components/AccelAndSpeed.h>
#include <Components/AnchorSelector.h>
#include <Components/Body.h>
#include <Components/CameraGuide.h>
#include <Components/Controllable.h>
#include <Components/GrappleControl.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#include <Components/Mass.h>

#include <Systems/Control.h>
#include <Systems/ControlAnchorSight.h>
#include <Systems/Gravity.h>
#include <Systems/PendulumSimulation.h>
#include <Systems/Render.h>

#include <aunteater/Entity.h>
#include <aunteater/UpdateTiming.h>


namespace ad {
debug::DrawDebugStuff * debugDrawer;
namespace grapkimbo {


static constexpr Color gAnchorColor{200, 200, 200};

Game_pendulum::Game_pendulum(Application & aApplication)
{
    debugDrawer = new debug::DrawDebugStuff(aApplication);
    mSystemManager.add<Control>();
    mSystemManager.add<PendulumSimulation>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<AccelSolver>();
    mSystemManager.add<ControlAnchorSight>(); // it will position the sight, which might follow something impacted by speed resolution
    mSystemManager.add<Render>(aApplication); 

    // Environment anchors
    aunteater::weak_entity anchor_1 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{4., 6.}, math::Size<2, double>{2., 2.} )
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::STATIC,
                ShapeType::HULL
                )
            .add<VisualRectangle>(gAnchorColor)
        );

    aunteater::weak_entity anchor_2 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{12., 5.}, math::Size<2, double>{2., 2.} )
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::STATIC,
                ShapeType::HULL
                )
            .add<VisualRectangle>(gAnchorColor)
        );

    aunteater::weak_entity anchor_3 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{24., 9.}, math::Size<2, double>{2., 2.} )
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, {2., 2.}},
                BodyType::STATIC,
                ShapeType::HULL
                )
            .add<VisualRectangle>(gAnchorColor)
        );

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::Keyboard;

    mEntityManager.addEntity(
        makePlayer(0, controller, math::sdr::gCyan, makePendular(anchor_1))
            .add<CameraGuide>());

    // Player 2
    if (isGamepadPresent(Controller::Gamepad_1))
    {
        mEntityManager.addEntity(
            makePlayer(1, Controller::Gamepad_1, math::sdr::gMagenta, makePendular(anchor_3)));
    }
}

bool Game_pendulum::update(const aunteater::Timer & aTimer, const GameInputState & aInputState)
{
    aunteater::UpdateTiming<GameInputState> timings;
    mSystemManager.update(aTimer, aInputState, timings);

    return ! mSystemManager.isPaused();
}

} // namespace grapkimbo
} // namespace ad
