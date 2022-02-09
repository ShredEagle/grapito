#pragma once

#include "RuleBase.h"

#include "../CameraGuidedControl.h" // for Camera archetype

#include "../../Components/Position.h"
#include "../../Components/PlayerData.h"

#include "../../Scenery/StateMachine.h"

#include <aunteater/FamilyHelp.h>

#include <handy/random.h>

#include <array>


namespace ad {
namespace grapito {


using Competitor = aunteater::Archetype<CameraGuide, PlayerData, Position>;

// This is a very naive implementation.
// There are many better options with a single container.
class PositionsBucket
{
public:
    PositionsBucket(std::initializer_list<Position2> aOptions) :
        mAvailable{std::move(aOptions)},
        mIndexer{0, std::numeric_limits<int>::max()}
    {}

    Position2 getRandom()
    {
        assert (mAvailable.size() > 0);

        auto candidate = mAvailable.begin() + (mIndexer() % mAvailable.size());
        Position2 result = *candidate;
        mAvailable.erase(candidate);
        mInUse.push_back(result);
        return result;
    }

    void reset()
    {
        std::ranges::copy(mInUse, std::back_inserter(mAvailable));
        mInUse.clear();
    }

private:
    std::vector<Position2> mAvailable;
    std::vector<Position2> mInUse;
    Randomizer<> mIndexer;
};


class CompetitionRule : public RuleBase
{
    enum Phase
    {
        Warmup,
        Competition,
        Congratulation,
        ExpectPlayers,
        FadeOut,
        FadeIn,

        _End,
    };

    using PhasesArray = std::array<std::shared_ptr<State>, _End>;

    friend PhasesArray setupGamePhases(std::shared_ptr<Context>, CompetitionRule &);

    template <class>
    friend class PhaseBase;
    template <class>
    friend class MessagePhase;
    friend class ExpectPlayersPhase;
    friend class WarmupPhase;
    friend class CompetitionPhase;
    friend class FadeOutPhase;
    friend class FadeInPhase;

public:
    CompetitionRule(aunteater::EntityManager & aEntityManager,
                    std::shared_ptr<Context> aContext,
                    std::shared_ptr<Control> aControlSystem,
                    std::shared_ptr<RenderToScreen> aRenderToScreenSystem);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    /// \brief Reset all competitors to their initial state.
    void resetCompetitors();

    void addNewCompetitors(bool aPreserveCameraPosition = false);

    /// \brief Remove all competitors from the game.
    void killAllCompetitors(); // Can you tell I watched squid game recently?

    /// \brief Apply the elimination rule to each competitor, removing competitors failing the test.
    /// \return The number of competitors remaining in the game after this step of eliminations.
    std::size_t eliminateCompetitors();

    aunteater::Entity prepareCameraFadeOut(Position2 aCameraPosition,
                                           const Position & aGeometry,
                                           CameraGuide & aCameraGuide);

    const aunteater::FamilyHelp<Competitor> mCompetitors;
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoint> mCameraPoints;

    // Values from PlayerControllerState.mPlayerSlot
    std::set<int> mAddedCompetitors;

    PositionsBucket mCandidatePositions{
        {-15.f, 3.f},
        {-11.f, 3.f},
        { -7.f, 3.f},
        { -3.f, 3.f},
        {  3.f, 3.f},
        {  7.f, 3.f},
        { 11.f, 3.f},
        { 15.f, 3.f},
    };

    PhasesArray mPhases;
    StateMachine mPhaseMachine;
};


} // namespace grapito
} // namespace ad
