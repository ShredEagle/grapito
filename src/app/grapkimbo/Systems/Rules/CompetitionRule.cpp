#include "CompetitionRule.h"

#include "Phases.h"

#include "../../Configuration.h"
#include "../../Context/Context.h"
#include "../../Entities.h"
#include "../../Logging.h"
#include "../../Timer.h"

#include "../../Components/Controllable.h"
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

const StringId hud_pressstart_sid = handy::internalizeString("hud_pressstart");
const StringId hud_joining_sid = handy::internalizeString("hud_joining");


using PhaseParent = PhaseBase<CompetitionRule>;


class CongratulationPhase : public MessagePhase<CompetitionRule>
{
    using Base_type = MessagePhase<CompetitionRule>;
public:
    using Base_type::Base_type;

    void beforeEnter() override
    {
        Base_type::beforeEnter();
        mRule.incrementScore();
    }
};


class ExpectPlayersPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    // Semantically is a `beforeEnter()`, but we do not want this to be
    // called before the CompetitionRule construction is complete
    // (this is the initial machine state).
    std::pair<TransitionProgress, UpdateStatus> enter(
        const GrapitoTimer &,
        const GameInputState &,
        const StateMachine &) override
    {
        mRule.resetCompetitors(); // in order to add at least the player that entered the game
        mRule.disableGrapples();
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(hud_waitplayers_sid),
                                                            hud::gCountdownPosition,
                                                            ScreenPosition::Center));
        return {TransitionProgress::None, UpdateStatus::KeepFrame};
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
        const GameInputState & /*aInputs*/,
        StateMachine & aStateMachine) override
    {
        updateImpl(aTimer.delta(), aStateMachine);
        // Note: Has no effect, nested state machines update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void updateImpl(float aDelta, StateMachine & aStateMachine)
    {
        mRule.instantiateQueuedCompetitors(true);
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
    mCompetitors{mEntityManager},
    mCameras{mEntityManager},
    mCameraPoints{mEntityManager},
    mHudLine{mEntityManager},
    mPhases{setupGamePhases(mContext, *this)},
    // ATTENTION the state machine is entering the first state directly, before CompetitionRule is done cting.
    mPhaseMachine{mPhases[ExpectPlayers]}
{}


void CompetitionRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    updateConnectedControllers();
    updatePlayerQueue();
    mPhaseMachine.update(aTimer, aInput);
    mHudLine.update();
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

            // Remove the player from the game, and queue it to be instantiated on next opportunity.
            // TODO This is a good example of circumventing the type system with entities:
            // provide a competitor where the archetype of a player is expected.
            killAndQueue(competitor);
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


void CompetitionRule::incrementScore()
{
    for(auto & [controller, playerStatus] : mControllerToPlayerStatus)
    {
        if (playerStatus.status == PlayerStatus::Playing)
        {
            ++playerStatus.score;
        }
    }
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
    instantiateQueuedCompetitors();
}


void CompetitionRule::updateConnectedControllers()
{
    for(Controller gamepad : listPresentGamepads())
    {
        auto [value, inserted] = mControllerToPlayerStatus.emplace(gamepad, PlayerStatus{mContext});
        if (inserted)
        {
            mHudLine.add(value->second);
        }
    }
}


void CompetitionRule::updatePlayerQueue()
{
    for (PlayerControllerState & player : mContext->mPlayerList)
    {
        if (player.mJoinState == PlayerJoinState_Queued)
        {
            mControllerToPlayerStatus.at(player.mControllerId).status = PlayerStatus::Queued;
        }
    }
}


void CompetitionRule::instantiateQueuedCompetitors(bool aPreserveCameraPosition)
{
    for (PlayerControllerState & player : mContext->mPlayerList)
    {
        auto & playerStatus = mControllerToPlayerStatus.at(player.mControllerId);

        // Note we do not keep the context player state in sync with the local player status
        // In particular, we do not move the context back to queued, ever.
        if (player.mJoinState == PlayerJoinState_Queued)
        {
            // Advance queued context players to playing state, which is used to control pause menu for e.g.
            // This is feedback from downstream to the context, which is maybe not good architecture.
            player.mJoinState = PlayerJoinState_Playing;
            mHudLine.setColor(playerStatus, player.mColor);
        }

        if (playerStatus.status == PlayerStatus::Queued || playerStatus.status == PlayerStatus::Eliminated)
        {
            playerStatus.status = PlayerStatus::Playing;

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
        }
    }
}


void CompetitionRule::killAndQueue(aunteater::weak_entity aCompetitor)
{
    mControllerToPlayerStatus.at(aCompetitor->get<Controllable>().controller).status = PlayerStatus::Eliminated;
    kill(aCompetitor);
}

void CompetitionRule::killAllCompetitors()
{
    for (auto competitor : mCompetitors)
    {
        killAndQueue(competitor);
    }
}

//
// Scoring and Hud
//
std::string PlayerStatus::text()
{
    switch(status)
    {
    case Connected:
        return mContext->translate(hud_pressstart_sid);
        break;
    case Queued:
        return mContext->translate(hud_joining_sid);
        break;
    case Eliminated:
    case Playing:
        return std::to_string(score);
        break;
    }
}


PlayerHud::PlayerHud(PlayerStatus & aStatus, aunteater::EntityManager & aEntityManager) :
    status{aStatus},
    hudText{aEntityManager.addEntity(
        makeHudText("", hud::gAltimeterPosition, ScreenPosition::Center))}
{
    hudText->get<Text>().size = Text::Small;
}

void PlayerHud::setColor(math::sdr::Rgb aColor)
{
    hudText->get<Text>().color = aColor;
}

void PlayerHud::update()
{
    hudText->get<Text>().message = status.text();
}


void HudLine::add(PlayerStatus & aStatus)
{
    mHuds.emplace_back(aStatus, mEntityManager);
    reflow();
}

void HudLine::setColor(PlayerStatus & aStatus, math::sdr::Rgb aColor)
{
    for (auto & hud : mHuds)
    {
        if(&hud.status == &aStatus)
        {
            hud.setColor(aColor);
            break;
        }
    }
}

void HudLine::reflow()
{
    const float xSpacing = game::gAppResolution.width() / (mHuds.size() + 1);
    int id = 1;
    for (auto & hud : mHuds)
    {
        hud.hudText->get<ScreenPosition>().position.x() = 
            -(game::gAppResolution.width() / 2)
            + id * xSpacing;
        ++id;
    }
}


} // namespace grapito
} // namespace ad
