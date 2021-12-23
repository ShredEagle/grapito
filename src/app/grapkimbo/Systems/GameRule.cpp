#include "GameRule.h"

#include "../Configuration.h"
#include "../Entities.h"
#include "../Logging.h"

#include "../Utils/Camera.h"


namespace ad {
namespace grapito {


class PhaseBase : public State
{
public:
    PhaseBase(GameRule & aGameRule) :
        mGameRule{aGameRule}
    {}

protected:
    std::shared_ptr<State> getPhase(GameRule::Phase aPhase)
    {
        return mGameRule.mPhases[aPhase];
    }

    GameRule & mGameRule;
};


class CompetitionPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;

    UpdateStatus update(
        GrapitoTimer &,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override
    {
        mGameRule.eliminateCompetitors();

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }
};


class FreeSoloPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;

    UpdateStatus update(
        GrapitoTimer &,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override
    {
        // TODO This should be handled for all active controllers, there might not even be a gamepad.
        if (aInputs.get(Controller::Gamepad_0)[Start].positiveEdge())
        {
            aStateMachine.putNext(getPhase(GameRule::Competition));
            aStateMachine.popState();
        }
        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }
};


GameRule::PhasesArray setupGamePhases(GameRule & aGameRule)
{
    GameRule::PhasesArray result;
    result[GameRule::FreeSolo] = std::make_shared<FreeSoloPhase>(aGameRule);
    result[GameRule::Competition] = std::make_shared<CompetitionPhase>(aGameRule);
    return result;
}


GameRule::GameRule(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCompetitors{aEntityManager},
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager},
    mPhases{setupGamePhases(*this)},
    mPhaseMachine{mPhases[FreeSolo]}
{}


void GameRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    //mPhaseMachine.update(aTimer, aInput);
}


void GameRule::eliminateCompetitors()
{
    // There is the one camera
    Position2 cameraPosition = mCameras.begin()->get<Position>().position;

    for (auto competitor : mCompetitors)
    {
        auto & [cameraGuide, playerData, geometry] = competitor;            
        if (geometry.position.y() < (cameraPosition.y() - game::gCompetitorEliminationDistance))
        {
            ADLOG(info)("Player {} eliminated!", playerData.id);

            // Patch the camera guide:
            // A player has a CameraGuide, but also CameraLimits, and the limits override the position computed from the guide.
            // A fade-out-guide entity will be added to smoothly transition the camera once the competitor is removed, but 
            // it will not use CameraLimits. So we need to compute the equivalent guide position.
            Influence cameraInfluence = accumulateCameraGuides(mCameraPoints);
            Vec2 withoutEliminated = cameraInfluence.accumulatedPosition - weightedGuideContribution(geometry, cameraGuide);

            // The guide position that should be added to keep the camera position unchanged, after the competitor entity is removed.
            Position2 patchedGuidePosition = (cameraPosition * cameraInfluence.totalWeight - withoutEliminated) / cameraGuide.influence;
            // The fade-out-guide will be copied from the player CameraGuide: write the result there.
            // with the added benefit that if several competitors are elmininated in the same step, this will remain correct.
            cameraGuide.offset = patchedGuidePosition - geometry.position;

            // Remove the player from the game, and 
            // TODO This is a good example of circumventing the type system with entities:
            // provide a competitor where the archetype of a player is expected.
            kill(competitor, mEntityManager);
        }

    }
}


} // namespace grapitor
} // namespace ad
