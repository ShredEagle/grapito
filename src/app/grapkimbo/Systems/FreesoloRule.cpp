#include "FreesoloRule.h"

#include "../Entities.h"
#include "../Timer.h"

#include "../Context/Context.h"
#include "../Context/PlayerList.h"


namespace ad {
namespace grapito {


FreesoloRule::FreesoloRule(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    mEntityManager{aEntityManager},
    mContext{std::move(aContext)},
    mControlSystem{std::move(aControlSystem)},
    mRenderToScreenSystem{std::move(aRenderToScreenSystem)}
{
    PlayerControllerState player = *mContext->mPlayerList.begin();
    mEntityManager.addEntity(
        makePlayingPlayer(player.mPlayerSlot, player.mControllerId, player.mColor, {0.f, 3.f}));
}


void FreesoloRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    // No specific rules at the moment, just execution of all the systems.
} 


} // namespace grapito
} // namespace ad
