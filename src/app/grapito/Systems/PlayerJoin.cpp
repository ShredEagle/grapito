#include "PlayerJoin.h"

#include "../Configuration.h"
#include "../Logging.h"
#include "../Timer.h"
#include "Entities.h"
#include "commons.h"


namespace ad {
namespace grapito {


PlayerJoin::PlayerJoin(aunteater::EntityManager & aEntityManager, std::shared_ptr<Context> aContext) :
    mPlayingPlayer{aEntityManager},
    mContext{aContext},
    mEntityManager{aEntityManager}
{}


void PlayerJoin::update(const GrapitoTimer, const GameInputState & aInputs)
{
    for (auto controller : mContext->mPlayerList.getUnactiveControllers())
    {
        if (aInputs.get(controller)[Command::Start].positiveEdge())
        {
            //Change player to Choosing color state
            mContext->mPlayerList.addPlayer(controller, PlayerJoinState_Queued);
        }
    }

    for (auto player : mPlayingPlayer)
    {
        auto controllable = player->get<Controllable>();

        if (!(controllable.controller == Controller::KeyboardMouse) && !isGamepadPresent(controllable.controller))
        {
            ADLOG(info)("Removing player {} from the list.", player->get<PlayerData>().id);
            mContext->mPlayerList.removePlayer(controllable.controller);
            mEntityManager.markToRemove(player);
        }
    }
}


} // namespace grapito
} // namespace ad
