#include "GameScene.h"

#include "Input.h"
#include "MenuScene.h"

#include "../Configuration.h"
#include "../Logging.h"
#include "../TopLevelStates.h"

#include "../Utils/MenuControls.h"
    

namespace ad {
namespace grapito {


GameScene::GameScene(std::shared_ptr<Context> aContext,
                     std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mAppInterface{std::move(aAppInterface)},
    mContext{std::move(aContext)}
{}


UpdateStatus GameScene::update(
    const GrapitoTimer & aTimer,
    const GameInputState & aInputs,
    StateMachine & aStateMachine)
{
    Controller pressingStartController = isPositiveEdge(aInputs, Start);
    if (pressingStartController != Controller::_End)
    {
        if (mContext->mPlayerList.getPlayerState(pressingStartController) != PlayerJoinState_NotActive)
        {
            aStateMachine.pushState(setupPauseMenu(mContext, mAppInterface, shared_from_this()));
            // Causes troubles with detection of next press of pause button
            // it would still be the same edge!
            //return aStateMachine.update(aTimer, aInputs);
            return UpdateStatus::KeepFrame;
        }
    }

    InputState debugPause = aInputs.get(Controller::KeyboardMouse)[Command::Pause];
    InputState debugStep  = aInputs.get(Controller::KeyboardMouse)[Command::Step];

    if (debugPause.positiveEdge())
    {
        mSystemManager.togglePause();
    }

    if (!mSystemManager.isPaused() || debugStep.positiveEdge())
    {
        // Make a copy, so the timer values can be modified for stepping.
        GrapitoTimer timerCopy = aTimer;
        if (mSystemManager.isPaused())
        {
            // TODO
            // The delta will be correct, but the resulting simulation time might not be
            // if current delta() (computed from real time in main) is not zero.
            timerCopy.mark(timerCopy.simulationTime() + debug::gStepTimeIncrement);
        }

        mSystemManager.step(timerCopy, aInputs, mUpdater);
        log(mUpdater);
        return UpdateStatus::SwapBuffers;
    }

    return UpdateStatus::KeepFrame;
}


} // namespace grapito
} // namespace ad
