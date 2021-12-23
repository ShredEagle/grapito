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
    GameRule(aunteater::EntityManager & aEntityManager, std::vector<aunteater::Entity> aPlayers);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    /// \brief Remove all competitors from the game.
    void killAllCompetitors(); // Can you tell I watched squid game recently?
    /// \brief Apply the elimination rule to each competitor, removing competitors failing the test.
    void eliminateCompetitors();

    void prepareCameraFadeOut(Position2 aCameraPosition, const Position & aGeometry, CameraGuide & aCameraGuide);

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Competitor> mCompetitors;
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoint> mCameraPoints;

    std::vector<aunteater::Entity> mPlayers;

    PhasesArray mPhases;
    StateMachine mPhaseMachine;
};


} // namespace grapito
} // namespace ad
