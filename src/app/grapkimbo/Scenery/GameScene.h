#include "StateMachine.h"

#include "../Utils/TimeSystemsUpdater.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>


namespace ad {
namespace grapito {


class GameScene : public State
{
public:
    UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

protected:
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GrapitoTimer, GameInputState> mSystemManager{mEntityManager};

private:
    TimeSystemsUpdater mUpdater;
}; 


} // namespace grapito
} // namespace ad
