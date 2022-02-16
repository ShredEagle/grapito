#pragma once


#include "GrappleJointCreator.h" // for RopeCreateArchetype

#include "../Input.h"

#include "../Components/PlayerData.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {


using PlayerDataArchetype = aunteater::Archetype<PlayerData>;

/// \brief Handles inter-player interaction of cutting ropes with grapples.
class GrappleInteractions : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    GrappleInteractions(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<RopeCreatorArchetype> mRopeCreators;
    const aunteater::FamilyHelp<PlayerDataArchetype> mPlayerDatas;
};


} // namespace grapito
} // namespace ad
