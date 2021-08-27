#include "Game_pendulum.h"

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
    mEntityManager.addEntity(
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

    mEntityManager.addEntity(
        aunteater::Entity()
            .add<Position>(math::Position<2, double>{24., 9.}, math::Size<2, double>{2., 2.} )
            .add<EnvironmentCollisionBox>(math::Rectangle<double>{{0., 0.}, {2., 2.}})
            .add<VisualRectangle>(gAnchorColor)
        );

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::Keyboard;

    aunteater::weak_entity player_1 = mEntityManager.addEntity(
        aunteater::Entity()
            .add<Controllable>(controller)
            .add<GrappleControl>(GrappleMode::AnchorSight)
            .add<Pendular>(Pendular{ {5., 6.}, math::Radian<double>{math::pi<double>/3.}, 3. })
            .add<Position>(math::Position<2, double>{0., 0.}, math::Size<2, double>{0.8, 1.9}) // The position will be set by pendulum simulation
            .add<VisualRectangle>(math::sdr::gCyan)
            .add<Weight>(80.)
        );

    // Player 1 sight
    mEntityManager.addEntity(
        aunteater::Entity()
            .add<AnchorSelector>(20., math::Degree<double>{35.}, player_1, anchor_2)
            .add<Controllable>(controller)
            .add<Position>() // Will be handled by ControlAnchorSight system
            .add<VisualOutline>(player_1->get<VisualRectangle>().color, 0.3)
    );

    // Player 2
    if (isGamepadPresent(Controller::Gamepad_1))
    {
        mEntityManager.addEntity(
            aunteater::Entity()
                .add<Position>(math::Position<2, double>{0., 0.}, math::Size<2, double>{0.8, 1.9}) // The position will be set by pendulum simulation
                .add<VisualRectangle>(math::sdr::gMagenta)
                .add<Pendular>(Pendular{ {25., 9.}, math::Radian<double>{-math::pi<double>/3.}, 3. })
                .add<Controllable>(Controller::Gamepad_1)
                .add<Weight>(80.)
            );
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
