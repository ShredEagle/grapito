#pragma once

#include "../Input.h"

#include <Components/PlayerData.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito
{

typedef aunteater::Archetype<PlayerData> Cleanupable;

class GrappleCleanup : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    GrappleCleanup(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<Cleanupable> mCleanable;
};


} // namespace grapito
} // namespace ad
