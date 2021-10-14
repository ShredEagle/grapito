#include "StateMachine.h"

#include "../Utils/TimeSystemsUpdater.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>

#include <graphics/AppInterface.h>


namespace ad {
namespace grapito {


class GameScene : public State
{
public:
    GameScene(std::shared_ptr<graphics::AppInterface> aAppInterface);

    UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

protected:
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GrapitoTimer, GameInputState> mSystemManager{mEntityManager};
    std::shared_ptr<graphics::AppInterface> mAppInterface;

private:
    TimeSystemsUpdater mUpdater;
}; 


} // namespace grapito
} // namespace ad
