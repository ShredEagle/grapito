#include "Game_pendulum.h"
#include "Configuration.h"
#include "Entities.h"

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

#include <Systems/AccelSolver.h>
#include <Systems/CameraGuidedControl.h>
#include "Systems/ContactConstraintCreation.h"
#include <Systems/Control.h>
#include <Systems/ControlAnchorSight.h>
#include <Systems/Gravity.h>
#include "Systems/ImpulseSolver.h"
#include <Systems/LevelGeneration.h>
#include <Systems/PendulumSimulation.h>
#include <Systems/Render.h>

#include <aunteater/Entity.h>
#include <aunteater/UpdateTiming.h>


namespace ad {

bool pause = false;

namespace grapito {

Game_pendulum::Game_pendulum(Application & aApplication)
{

    mSystemManager.add<LevelGeneration>();
    mSystemManager.add<Control>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<PendulumSimulation>();
    mSystemManager.add<ControlAnchorSight>(); // it will position the sight, which might follow something impacted by speed resolution
    mSystemManager.add<CameraGuidedControl>();
    mSystemManager.add<ContactConstraintCreation>();

    mSystemManager.add<AccelSolver>();
    mSystemManager.add<ImpulseSolver>();

    mSystemManager.add<Render>(aApplication); 

    // Camera
    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera());
    // Environment anchors
    aunteater::weak_entity anchor_1 = mEntityManager.addEntity(
        makeAnchor(Position2{-1., -1.}, math::Size<2, double>{2., 2.}));

    aunteater::weak_entity anchor_2 = mEntityManager.addEntity(
        makeAnchor(Position2{12., 5.}, math::Size<2, double>{2., 2.} ));

    aunteater::weak_entity anchor_3 = mEntityManager.addEntity(
        makeAnchor(Position2{24., 9.}, math::Size<2, double>{2., 2.} ));

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::Keyboard;

    mEntityManager.addEntity(
        makePlayer(0, controller, math::sdr::gCyan, makePendular(anchor_1))
            .add<CameraGuide>(1.0));

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

} // namespace grapito
} // namespace ad