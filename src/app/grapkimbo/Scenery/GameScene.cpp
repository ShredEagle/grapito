#include "GameScene.h"

#include "MenuScene.h"

#include "../Configuration.h"
#include "../Logging.h"
#include "../TopLevelStates.h"
    

namespace ad {
namespace grapito {


GameScene::GameScene(std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mAppInterface{std::move(aAppInterface)}
{}


UpdateStatus GameScene::update(
    GrapitoTimer & aTimer,
    const GameInputState & aInputs,
    StateMachine & aStateMachine)
{
    InputState gamePause = aInputs.get(Controller::Keyboard)[Command::Start];
    if (gamePause.positiveEdge())
    {
        aStateMachine.pushState(setupPauseMenu(mAppInterface));
        // Causes troubles with detection of next press of pause button
        // it would still be the same edge!
        //return aStateMachine.update(aTimer, aInputs);
        return UpdateStatus::KeepFrame;
    }

    InputState debugPause = aInputs.get(Controller::Keyboard)[Command::Pause];
    InputState debugStep  = aInputs.get(Controller::Keyboard)[Command::Step];

    if (debugPause.positiveEdge())
    {
        mSystemManager.togglePause();
    }

    if (!mSystemManager.isPaused() || debugStep.positiveEdge())
    {
        if (mSystemManager.isPaused())
        {
            // The delta will be correct, but the resulting simulation time might not be
            // if current delta() (computer from real time in main) is not zero.
            aTimer.mark(aTimer.simulationTime() + debug::gStepTimeIncrement);
        }

        mSystemManager.step(aTimer, aInputs, mUpdater);
        log(mUpdater);
        return UpdateStatus::SwapBuffers;
    }

    return UpdateStatus::KeepFrame;
}


} // namespace grapito
} // namespace ad