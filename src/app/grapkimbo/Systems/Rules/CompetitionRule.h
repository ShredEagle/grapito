#pragma once

#include "RuleBase.h"

#include "../CameraGuidedControl.h" // for Camera archetype

#include "../../Components/Position.h"
#include "../../Components/PlayerData.h"

#include "../../Context/PlayerList.h"

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


struct PlayerStatus
{
    enum Status
    {
        Connected,

        Queued,
        Eliminated,
        Playing,
    };

    std::string text();

    std::shared_ptr<Context> mContext;
    Status status{Connected};
    int score{0};
};


struct PlayerHud
{
    //PlayerHud(math::sdr::Rgb aColor, aunteater::EntityManager & aEntityManager) :
    //    mHudText{aEntityManager.addEntity(
    //        makeHudText("", hud::gAltimeterPosition, ScreenPosition::Center))},
    //{
    //    mHudText->get<Text>().color = aColor;
    //}

    PlayerHud(PlayerStatus & aStatus, aunteater::EntityManager & aEntityManager);

    void update();

    PlayerStatus & status;
    aunteater::weak_entity hudText{nullptr};
};


class HudLine
{
public:
    HudLine(aunteater::EntityManager & aEntityManager) :
        mEntityManager{aEntityManager}
    {}

    void add(PlayerStatus & aStatus);

    void update()
    {
        std::ranges::for_each(mHuds, &PlayerHud::update);
    }

private:
    void reflow();

    aunteater::EntityManager & mEntityManager;
    std::vector<PlayerHud> mHuds;
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
    void updateConnectedControllers();

    void updatePlayerQueue(PlayerJoinState aStateToQueueFrom = PlayerJoinState_Queued);

    /// \brief Reset all competitors to their initial state.
    void resetCompetitors();

    void instantiateQueuedCompetitors(bool aPreserveCameraPosition = false);

    /// \rief Remove a single competitor from the game
    void killAndQueue(aunteater::weak_entity aCompetitor);

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

    std::map<Controller, PlayerStatus> mControllerToPlayerStatus;

    HudLine mHudLine;

    PhasesArray mPhases;
    StateMachine mPhaseMachine;
};


} // namespace grapito
} // namespace ad
