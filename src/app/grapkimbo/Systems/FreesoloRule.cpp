#include "FreesoloRule.h"

#include "../Entities.h"
#include "../Timer.h"

#include "../Components/Position.h"
#include "../Components/Text.h"

#include "../Context/Context.h"
#include "../Context/PlayerList.h"

#include <numeric>


namespace ad {
namespace grapito {


FreesoloRule::FreesoloRule(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    mEntityManager{aEntityManager},
    mContext{std::move(aContext)},
    mHudText{mEntityManager.addEntity(
        makeHudText("", hud::gAltimeterPosition, ScreenPosition::Center))},
    mControlSystem{std::move(aControlSystem)},
    mRenderToScreenSystem{std::move(aRenderToScreenSystem)}
{
    mHudText->get<Text>().size = Text::Small;

    PlayerControllerState player = *mContext->mPlayerList.begin();
    mPlayer = mEntityManager.addEntity(
        makePlayingPlayer(player.mPlayerSlot, player.mControllerId, player.mColor, {0.f, 3.f}));
}


void FreesoloRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    mHeights.pop_front();
    mHeights.push_back(std::max(0.f, std::floor(mPlayer->get<Position>().position.y())));

    int average = std::accumulate(mHeights.begin(), mHeights.end(), 0) / mHeights.size();
    mHudText->get<Text>().message = std::to_string(average);
} 


} // namespace grapito
} // namespace ad
