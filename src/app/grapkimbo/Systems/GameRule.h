#pragma once

#include "CameraGuidedControl.h" // for Camera archetype

#include "../Components/Position.h"
#include "../Components/PlayerData.h"

#include "../Scenery/StateMachine.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <array>


namespace ad {
namespace grapito {


using Competitor = aunteater::Archetype<CameraGuide, PlayerData, Position>;


class GameRule : public aunteater::System<GrapitoTimer, GameInputState>
{
    enum Phase
    {
        FreeSolo,
        Competition, 

        _End,
    };

    using PhasesArray = std::array<std::shared_ptr<State>, _End>;

    friend PhasesArray setupGamePhases(GameRule &);

    friend class PhaseBase;
    friend class FreeSoloPhase;
    friend class CompetitionPhase;

public:
    GameRule(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    void eliminateCompetitors();

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Competitor> mCompetitors;
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoint> mCameraPoints;

    PhasesArray mPhases;
    StateMachine mPhaseMachine;
};


} // namespace grapito
} // namespace ad
