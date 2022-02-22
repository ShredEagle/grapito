#pragma once

#include "Input.h"
#include "commons.h"

#include "../Context/Context.h"

#include "../Components/Body.h"
#include "../Components/Controllable.h"
#include "../Components/PlayerData.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {

using PlayingPlayer = aunteater::Archetype<PlayerData, Controllable, Body>;

class PlayerJoin : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    PlayerJoin(aunteater::EntityManager & aEntityManager, std::shared_ptr<Context> aContext);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<PlayingPlayer> mPlayingPlayer;
    std::shared_ptr<Context> mContext;
    aunteater::EntityManager & mEntityManager;
};


} // namespace grapito
} // namespace ad
