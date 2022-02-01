#include "PlayerJoin.h"

#include "../Configuration.h"
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


void PlayerJoin::update(const GrapitoTimer aTimer, const GameInputState & aInputs)
{
    for (auto controller : mContext->mPlayerList.getUnactiveControllers())
    {
        if (aInputs.get(controller)[Command::Start].positiveEdge())
        {
            //Change player to Choosing color state
            int slot = mContext->mPlayerList.addPlayer(controller, PlayerJoinState_Playing);
            if (slot != -1)
            {
                //Add Color choice Menu entity
                auto player = makePlayingPlayer(slot, controller, math::sdr::gCyan);

                mEntityManager.addEntity(player);
            }

        }
    }

    for (auto player : mPlayingPlayer)
    {
        auto controllable = player->get<Controllable>();

        if (!(controllable.controller == Controller::KeyboardMouse) && !isGamepadPresent(controllable.controller))
        {
            mContext->mPlayerList.removePlayer(controllable.controller);
            mEntityManager.markToRemove(player);
        }
    }
}


} // namespace grapito
} // namespace ad
