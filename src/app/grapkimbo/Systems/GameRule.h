#pragma once

#include "CameraGuidedControl.h" // for Camera archetype

#include "../Components/Position.h"
#include "../Components/PlayerData.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {


using Competitor = aunteater::Archetype<CameraGuide, PlayerData, Position>;


class GameRule : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    GameRule(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Competitor> mCompetitors;
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoint> mCameraPoints;
};


} // namespace grapito
} // namespace ad
