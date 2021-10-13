#include "GameScene.h"

#include "../Logging.h"
    

namespace ad {
namespace grapito {

static constexpr float gDebugStepTimeIncrement = 0.016f;

bool GameScene::update(
    GrapitoTimer & aTimer,
    const GameInputState & aInputs,
    StateMachine & aStateMachine)
{
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
            aTimer.mark(aTimer.simulationTime() + gDebugStepTimeIncrement);
        }

        mSystemManager.step(aTimer, aInputs, mUpdater);
        log(mUpdater);
        return true;
    }

    return false;
}


} // namespace grapito
} // namespace ad
