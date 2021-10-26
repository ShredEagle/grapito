#pragma once

#include "../Input.h"

#include "../Components/PlayerData.h"
#include "../Components/Body.h"
#include "../Components/AccelAndSpeed.h"

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<AccelAndSpeed, Body> Massive;
typedef aunteater::Archetype<AccelAndSpeed, PlayerData> Player;

class Gravity : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Gravity(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    static constexpr double gAcceleration = 10.;

private:
    const aunteater::FamilyHelp<Massive> mMassives;
    const aunteater::FamilyHelp<Player> mPlayers;

};

} // namespace grapito
} // namespace ad
