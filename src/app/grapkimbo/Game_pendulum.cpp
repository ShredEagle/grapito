#include "Game_pendulum.h"
#include "Player.h"

#include <Components/AnchorSelector.h>
#include <Components/Controllable.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/ForceAndSpeed.h>
#include <Components/GrappleControl.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#include <Components/Weight.h>

#include <Systems/Control.h>
#include <Systems/ControlAnchorSight.h>
#include <Systems/Gravity.h>
#include <Systems/PendulumSimulation.h>
#include <Systems/Render.h>
#include <Systems/SpeedResolution.h>

#include <aunteater/Entity.h>
#include <aunteater/UpdateTiming.h>


namespace ad {
namespace grapkimbo {


static constexpr Color gAnchorColor{200, 200, 200};

Game_pendulum::Game_pendulum(Application & aApplication)
{
    mSystemManager.add<Control>();
    mSystemManager.add<ControlAnchorSight>();
    mSystemManager.add<PendulumSimulation>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<SpeedResolution>();
    mSystemManager.add<Render>(aApplication); 

    // Environment anchors
    aunteater::weak_entity anchor_1 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{4., 6.}, math::Size<2, double>{2., 2.} )
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {2., 2.}})
            .add<VisualRectangle>(gAnchorColor)
        );

    aunteater::weak_entity anchor_2 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{12., 5.}, math::Size<2, double>{2., 2.} )
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {2., 2.}})
            .add<VisualRectangle>(gAnchorColor)
        );

    aunteater::weak_entity anchor_3 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{24., 9.}, math::Size<2, double>{2., 2.} )
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {2., 2.}})
            .add<VisualRectangle>(gAnchorColor)
        );

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::Keyboard;

    mEntityManager.addEntity(
        makePlayer(0, controller, math::sdr::gCyan, makePendular(anchor_1)));

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
