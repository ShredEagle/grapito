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


class Control;


class GameRule : public aunteater::System<GrapitoTimer, GameInputState>
{
    enum Phase
    {
        FreeSolo,
        Warmup,
        Competition, 
        Congratulation,

        _End,
    };

    using PhasesArray = std::array<std::shared_ptr<State>, _End>;

    friend PhasesArray setupGamePhases(std::shared_ptr<Context>, GameRule &);

    friend class PhaseBase;
    friend class FreeSoloPhase;
    friend class WarmupPhase;
    friend class CompetitionPhase;
    friend class CongratulationPhase;

public:
    GameRule(aunteater::EntityManager & aEntityManager, std::shared_ptr<Context> aContext, std::vector<aunteater::Entity> aPlayers, std::shared_ptr<Control> aControlSystem);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    /// \brief Reset all competitors to their initial state.
    void resetCompetitors();

    /// \brief Remove all competitors from the game.
    void killAllCompetitors(); // Can you tell I watched squid game recently?

    /// \brief Apply the elimination rule to each competitor, removing competitors failing the test.
    /// \return The number of competitors remaining in the game after this step of eliminations.
    std::size_t eliminateCompetitors();

    void prepareCameraFadeOut(Position2 aCameraPosition, const Position & aGeometry, CameraGuide & aCameraGuide);

    void enableGrapples();
    void disableGrapples();

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Competitor> mCompetitors;
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoint> mCameraPoints;

    std::shared_ptr<Control> mControlSystem;

    std::vector<aunteater::Entity> mPlayers;

    PhasesArray mPhases;
    StateMachine mPhaseMachine;
};


} // namespace grapito
} // namespace ad
