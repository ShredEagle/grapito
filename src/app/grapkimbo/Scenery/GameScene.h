#include "StateMachine.h"

#include "../Context/Context.h"
#include "../Utils/TimeSystemsUpdater.h"

#include <aunteater/EntityManager.h>
#include <aunteater/SystemManager.h>

#include <graphics/AppInterface.h>

#include <memory>


namespace ad {
namespace grapito {


class GameScene : public State, public std::enable_shared_from_this<GameScene>
{
public:
    GameScene(std::shared_ptr<Context> aContext,
              std::shared_ptr<graphics::AppInterface> aAppInterface);

    /// \brief Complete update step, including rendering.
    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

    /// \brief Should render the game in the current state.
    ///
    /// It is intended to implement effect such as blurring during pause.
    virtual void render() const = 0;

protected:
    aunteater::EntityManager mEntityManager;
    aunteater::SystemManager<GrapitoTimer, GameInputState> mSystemManager{mEntityManager};
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    std::shared_ptr<Context> mContext;

private:
    TimeSystemsUpdater mUpdater;
}; 


} // namespace grapito
} // namespace ad
