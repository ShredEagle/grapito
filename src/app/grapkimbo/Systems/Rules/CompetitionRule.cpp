#include "CompetitionRule.h"

#include "Phases.h"

#include "../../Configuration.h"
#include "../../Context/Context.h"
#include "../../Entities.h"
#include "../../Logging.h"
#include "../../Timer.h"

#include "../../Components/ScreenPosition.h"
#include "../../Components/Text.h"

#include "../../Utils/Camera.h"

#include <handy/StringId_Interning.h>


namespace ad {
namespace grapito {


const StringId hud_waitplayers_sid  = handy::internalizeString("hud_waitplayers");
const StringId hud_victory_sid  = handy::internalizeString("hud_victory");
const StringId hud_solomode_sid = handy::internalizeString("hud_solomode");
const StringId hud_1_sid = handy::internalizeString("hud_1");
const StringId hud_2_sid = handy::internalizeString("hud_2");
const StringId hud_3_sid = handy::internalizeString("hud_3");
const StringId hud_climb_sid = handy::internalizeString("hud_climb");


using PhaseParent = PhaseBase<CompetitionRule>;

using CongratulationPhase = MessagePhase<CompetitionRule>;

class ExpectPlayersPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {
        mRule.resetCompetitors(); // in order to add at least the player that entered the game
        mRule.disableGrapples();
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(hud_waitplayers_sid),
                                                            hud::gCountdownPosition,
                                                            ScreenPosition::Center));
    }


    UpdateStatus update(
            const GrapitoTimer & aTimer,
            const GameInputState &,
            StateMachine & aStateMachine) override
    {
        mHudText->get<Text>().color.a() = mFlashing.advance(aTimer.delta()) * 255;

        if(mContext->mPlayerList.countActivePlayers() > 1)
        {
            aStateMachine.putNext(getPhase(CompetitionRule::Warmup));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }


    void beforeExit() override
    {
        mHudText->markToRemove();
    }

    aunteater::weak_entity mHudText{nullptr};
    math::Interpolation<float, float, math::periodic::PingPong> mFlashing =
        math::makeInterpolation<math::periodic::PingPong>(0.f, 1.f, game::gFadeDuration);
};


class FadeOutPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {
        mFadeOpacity.reset();
    }


    UpdateStatus update(
            const GrapitoTimer & aTimer,
            const GameInputState &,
            StateMachine & aStateMachine) override
    {
        mRule.setFadeOpacity(mFadeOpacity.advance(aTimer.delta()));
        if(mFadeOpacity.isCompleted())
        {
            aStateMachine.putNext(getPhase(CompetitionRule::FadeIn));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }


    void beforeExit() override
    {
        mRule.disableFade();
    }

    math::Interpolation<float, float> mFadeOpacity =
        math::makeInterpolation(0.f, 1.f, game::gFadeDuration);
};


/// \brief Reset players and fade in (i.e. show game world)
class FadeInPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {
        mFadeOpacity.reset();
        mRule.resetCompetitors();
        mRule.disableGrapples();
    }


    UpdateStatus update(
            const GrapitoTimer & aTimer,
            const GameInputState &,
            StateMachine & aStateMachine) override
    {
        mRule.setFadeOpacity(mFadeOpacity.advance(aTimer.delta()));
        if(mFadeOpacity.isCompleted())
        {
            aStateMachine.putNext(getPhase(CompetitionRule::Warmup));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }


    void beforeExit() override
    {
        mRule.disableFade();
    }

    math::Interpolation<float, float> mFadeOpacity =
        math::makeInterpolation(1.f, 0.f, game::gFadeDuration);
};


/// \brief Countdown during which grapples cannot be used.
class WarmupPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(mSteps.front()),
                                                            hud::gCountdownPosition,
                                                            ScreenPosition::Center));
    }

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override
    {
        updateImpl(aTimer.delta(), aStateMachine);
        // Note: Has no effect, nested state machines update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void updateImpl(float aDelta, StateMachine & aStateMachine)
    {
        mRule.addNewCompetitors(true);
        auto param = mCountdownParam.advance(aDelta);
        if(mCountdownParam.isCompleted())
        {
            ++mCurrentStep;
            auto overshoot = mCountdownParam.getOvershoot();
            mCountdownParam.reset();

            if(mCurrentStep == mSteps.size())
            {
                mCurrentStep = 0;
                aStateMachine.putNext(getPhase(CompetitionRule::Competition));
                aStateMachine.popState();
            }
            else
            {
                mHudText->get<Text>().message = mContext->translate(mSteps.at(mCurrentStep));
                updateImpl(overshoot, aStateMachine);
            }
        }
        else
        {
            mHudText->get<Text>().color.a() = param * 255;
        }
    }

    void beforeExit() override
    {
        mHudText->markToRemove();
        mRule.enableGrapples();
    }


    math::ParameterAnimation<float, math::Clamp> mCountdownParam =
        math::makeParameterAnimation<math::Clamp>(game::gCountdownStepPeriod);
    std::array<StringId, 3> mSteps{hud_3_sid, hud_2_sid, hud_1_sid};
    std::size_t mCurrentStep{0};
    aunteater::weak_entity mHudText{nullptr};
};



class CompetitionPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    // Should be a beforeEnter, but needs the timer
    std::pair<TransitionProgress, UpdateStatus> enter(
        const GrapitoTimer & aTimer,
        const GameInputState &,
        const StateMachine &) override
    {
        mEnterTime = aTimer.simulationTime();
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(hud_climb_sid),
                                                            hud::gCountdownPosition,
                                                            ScreenPosition::Center));
        return {TransitionProgress::Complete, UpdateStatus::SwapBuffers};
    }

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & /*aInputs*/,
        StateMachine & aStateMachine) override
    {
        if (mHudText && aTimer.simulationTime() > (mEnterTime + hud::gClimbMessageDuration))
        {
            (*mHudText)->markToRemove();
            mHudText = std::nullopt;
        }

        if (mRule.eliminateCompetitors() == 1)
        {
            // There is a winner
            ADLOG(info)("Climb over.");
            aStateMachine.putNext(getPhase(CompetitionRule::Congratulation));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void beforeExit() override
    {
        // Just in case the removal condition was not already met before exiting
        if (mHudText)
        {
            (*mHudText)->markToRemove();
        }
    }

    std::optional<aunteater::weak_entity> mHudText;
    float mEnterTime;
};


CompetitionRule::PhasesArray setupGamePhases(std::shared_ptr<Context> aContext, CompetitionRule & aCompetitionRule)
{
    CompetitionRule::PhasesArray result;
    result[CompetitionRule::Warmup] = std::make_shared<WarmupPhase>(aContext, aCompetitionRule);
    result[CompetitionRule::Competition] = std::make_shared<CompetitionPhase>(aContext, aCompetitionRule);
    result[CompetitionRule::Congratulation] = 
        std::make_shared<CongratulationPhase>(aContext, aCompetitionRule,
                                              aContext->translate(hud_victory_sid), CompetitionRule::FadeOut);
    result[CompetitionRule::ExpectPlayers] = std::make_shared<ExpectPlayersPhase>(aContext, aCompetitionRule);
    result[CompetitionRule::FadeOut] = std::make_shared<FadeOutPhase>(aContext, aCompetitionRule);
    result[CompetitionRule::FadeIn] = std::make_shared<FadeInPhase>(aContext, aCompetitionRule);
    return result;
}


CompetitionRule::CompetitionRule(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    RuleBase{aEntityManager, std::move(aContext), std::move(aControlSystem), std::move(aRenderToScreenSystem)},
    mCompetitors{aEntityManager},
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager},
    mPhases{setupGamePhases(mContext, *this)},
    // ATTENTION the state machine is entering the first state directly, before CompetitionRule is done cting.
    mPhaseMachine{mPhases[ExpectPlayers]}
{}


void CompetitionRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    mPhaseMachine.update(aTimer, aInput);
}


std::size_t CompetitionRule::eliminateCompetitors()
{
    // There is the one camera
    Position2 cameraPosition = mCameras.begin()->get<Position>().position;

    std::size_t remainingCompetitors = 0;
    std::vector<aunteater::Entity> fadeOutGuides;
    for (auto competitor : mCompetitors)
    {
        auto & [cameraGuide, playerData, geometry] = competitor;
        if (geometry.position.y() < (cameraPosition.y() - game::gCompetitorEliminationDistance))
        {
            ADLOG(info)("Player {} eliminated!", playerData.id);

            fadeOutGuides.push_back(prepareCameraFadeOut(cameraPosition, geometry, cameraGuide));

            // Remove the player from the game, and
            // TODO This is a good example of circumventing the type system with entities:
            // provide a competitor where the archetype of a player is expected.
            kill(competitor);
        }
        else
        {
            ++remainingCompetitors;
        }
    }

    // NOTE Ad 2022/02/07: The fade out guides are added to the simulation only once they are all calculated :
    // If they were added when they are computed, they would influence the computation of the next guide,
    // resulting in glitchy camera.
    for(const auto & guide : fadeOutGuides)
    {
        mEntityManager.addEntity(guide);
    }

    return remainingCompetitors;
}


aunteater::Entity CompetitionRule::prepareCameraFadeOut(Position2 aCameraPosition,
                                                 const Position & aGeometry,
                                                 CameraGuide & aCameraGuide)
{
    // Patch the camera guide:
    // A player has a CameraGuide, but also CameraLimits, and the limits override the position computed from the guide.
    // A fade-out-guide entity will be added to smoothly transition the camera once the competitor is removed, but
    // it will not use CameraLimits. So we need to compute the equivalent guide position.
    Influence cameraInfluence = accumulateCameraGuides(mCameraPoints);
    Vec2 withoutEliminated = cameraInfluence.accumulatedPosition - weightedGuideContribution(aGeometry, aCameraGuide);

    // The guide position that should be added to keep the camera position unchanged, after the competitor entity is removed.
    Position2 patchedGuidePosition = (aCameraPosition * cameraInfluence.totalWeight - withoutEliminated)
                                     / aCameraGuide.influence;
    // The fade-out-guide will be copied from the player CameraGuide: write the result there.
    // with the added benefit that if several competitors are elmininated in the same step, this will remain correct.
    aCameraGuide.offset = patchedGuidePosition - aGeometry.position;

    // Prepare the smooth fade-out camera transition.
    {
        CameraGuide smoothOut = aCameraGuide;
        smoothOut.influenceInterpolation =
            math::makeInterpolation<math::None, math::ease::SmoothStep>
                                   (smoothOut.influence, 0.f, camera::gCompetitorGuideFadeOutDuration);
        smoothOut.completionBehaviour = CameraGuide::OnCompletion::RemoveEntity;

        return aunteater::Entity{}
            .add<CameraGuide>(smoothOut)
            .add<Position>(aGeometry)
        ;
    }
}


void CompetitionRule::resetCompetitors()
{
    mCandidatePositions.reset();
    killAllCompetitors();
    mAddedCompetitors.clear();
    addNewCompetitors();
}


void CompetitionRule::addNewCompetitors(bool aPreserveCameraPosition)
{
    for (PlayerControllerState & player : mContext->mPlayerList)
    {
        // Advance queued players to playing state.
        if (player.mJoinState == PlayerJoinState_Queued)
        {
            player.mJoinState = PlayerJoinState_Playing;
        }

        if (!mAddedCompetitors.contains(player.mPlayerSlot)
            && player.mJoinState == PlayerJoinState_Playing)
        {
            Position2 aSpawnPos = [&]()
            {
                if (aPreserveCameraPosition)
                {
                    auto cameraGuides = accumulateCameraGuides(mCameraPoints);
                    auto result =
                        cameraGuides.accumulatedPosition.as<math::Position>() / cameraGuides.totalWeight
                        - player::gCameraGuideOffset;
                    result.y() = 3.f; // So it spawns at a known height (not traversing environment)
                    return result;
                }
                else
                {
                    return mCandidatePositions.getRandom();
                }
            }();

            mEntityManager.addEntity(
                makePlayingPlayer(player.mPlayerSlot, player.mControllerId, player.mColor, aSpawnPos));
            mAddedCompetitors.insert(player.mPlayerSlot);
        }
    }
}


void CompetitionRule::killAllCompetitors()
{
    for (auto competitor : mCompetitors)
    {
        kill(competitor);
    }
}


} // namespace grapito
} // namespace ad
