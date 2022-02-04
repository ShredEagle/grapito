#include "GameRule.h"

#include "Control.h"
#include "RenderToScreen.h"

#include "../Configuration.h"
#include "../Context/Context.h"
#include "../Entities.h"
#include "../Logging.h"
#include "../Timer.h"

#include "../Components/ScreenPosition.h"
#include "../Components/Text.h"

#include "../Utils/Camera.h"
#include "../Utils/CompositeTransition.h"

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


class PhaseBase : public State
{
public:
    PhaseBase(std::shared_ptr<Context> aContext, GameRule & aGameRule) :
        mContext{std::move(aContext)},
        mGameRule{aGameRule}
    {}

protected:
    std::shared_ptr<State> getPhase(GameRule::Phase aPhase)
    {
        return mGameRule.mPhases[aPhase];
    }

    aunteater::EntityManager & getEntityManager()
    {
        return mGameRule.mEntityManager;
    }

    std::shared_ptr<Context> mContext;
    GameRule & mGameRule;
};


class CongratulationPhase : public PhaseBase
{
    static constexpr Position2 gOutside = {0.f, -1000.f};

    static auto PrepareInterpolation()
    {
        CompositeTransition<Position2, float> result{gOutside};
        result
            .pushInterpolation<math::None>(game::gCongratulationScreenPosition,
                                           game::gCongratulationPhaseDuration * (1./3.))
            .pushConstant(game::gCongratulationPhaseDuration * (2./3.));
        return result;
    }

public:
    CongratulationPhase(std::shared_ptr<Context> aContext, GameRule & aGameRule) :
        PhaseBase{std::move(aContext), aGameRule},
        mHudPositionInterpolationReference{PrepareInterpolation()},
        mHudPositionInterpolation{mHudPositionInterpolationReference}
    {}

private:
    void resetInterpolation()
    {
        mHudPositionInterpolation = mHudPositionInterpolationReference;
        mFadeOpacity.reset();
    }

    void beforeEnter() override
    {
        resetInterpolation();
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(hud_victory_sid), 
                                                            gOutside,
                                                            ScreenPosition::Center));
    }


    UpdateStatus update(
            const GrapitoTimer & aTimer,
            const GameInputState & /*aInputs*/,
            StateMachine & aStateMachine) override
    {
        mHudText->get<ScreenPosition>().position =
            mHudPositionInterpolation.advance(aTimer.delta());
        if(mHudPositionInterpolation.isCompleted())
        {
            // This is advancing both the fade opacity and the position interpolation
            // during the transition frame (ideally, it should only advance opacity with the overshoot)
            // but that is acceptable
            mGameRule.setFadeOpacity(mFadeOpacity.advance(aTimer.delta()));
            if(mFadeOpacity.isCompleted())
            {
                aStateMachine.putNext(getPhase(GameRule::Warmup));
                aStateMachine.popState();
            }
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }


    void beforeExit() override
    {
        mHudText->markToRemove();
    }


    const CompositeTransition<Position2, float> mHudPositionInterpolationReference;
    CompositeTransition<Position2, float> mHudPositionInterpolation;

    math::Interpolation<float, float> mFadeOpacity = 
        math::makeInterpolation(0.f, 1.f, game::gFadeDuration);
    aunteater::weak_entity mHudText{nullptr};
};


class ExpectPlayersPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;

    void beforeEnter() override
    {
        mFadeOpacity.reset();
        mGameRule.disableGrapples();
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
            mGameRule.setFadeOpacity(mFadeOpacity.advance(aTimer.delta()));
            if(mFadeOpacity.isCompleted())
            {
                aStateMachine.putNext(getPhase(GameRule::Warmup));
                aStateMachine.popState();
            }
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }


    void beforeExit() override
    {
        mHudText->markToRemove();
        mGameRule.enableGrapples();
    }
    aunteater::weak_entity mHudText{nullptr};
    math::Interpolation<float, float, math::periodic::PingPong> mFlashing =
        math::makeInterpolation<math::periodic::PingPong>(0.f, 1.f, game::gFadeDuration);
    math::Interpolation<float, float> mFadeOpacity =
        math::makeInterpolation(0.f, 1.f, game::gFadeDuration);
};

/// \brief Countdown during which grapples cannot be used.
class WarmupPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;

    void beforeEnter() override
    {
        mGameRule.resetCompetitors();
        mGameRule.disableGrapples();
        mHudText = getEntityManager().addEntity(makeHudText(mContext->translate(mSteps.front()),
                                                            hud::gCountdownPosition,
                                                            ScreenPosition::Center));
        mFadeOpacity.reset();
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
        mGameRule.setFadeOpacity(mFadeOpacity.advance(aDelta));
        auto param = mCountdownParam.advance(aDelta);
        if(mCountdownParam.isCompleted())
        {
            ++mCurrentStep;
            auto overshoot = mCountdownParam.getOvershoot();
            mCountdownParam.reset();

            if(mCurrentStep == mSteps.size())
            {
                mCurrentStep = 0;
                aStateMachine.putNext(getPhase(GameRule::Competition));
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
        mGameRule.enableGrapples();
        mGameRule.disableFade();
    }


    math::ParameterAnimation<float, math::Clamp> mCountdownParam = 
        math::makeParameterAnimation<math::Clamp>(game::gCountdownStepPeriod);
    math::Interpolation<float, float> mFadeOpacity = 
        math::makeInterpolation(1.f, 0.f, game::gFadeDuration);
    std::array<StringId, 3> mSteps{hud_3_sid, hud_2_sid, hud_1_sid};
    std::size_t mCurrentStep{0};
    aunteater::weak_entity mHudText{nullptr};
};



class CompetitionPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;

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

        if (mGameRule.eliminateCompetitors() == 1)
        {
            // There is a winner
            ADLOG(info)("Climb over.");
            aStateMachine.putNext(getPhase(GameRule::Congratulation));
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


class FreeSoloPhase : public PhaseBase
{
    using PhaseBase::PhaseBase;


    void beforeEnter() override
    {
        mHudText = getEntityManager().addEntity(
            makeHudText(mContext->translate(hud_solomode_sid),
                        hud::gModeTextPosition,
                        ScreenPosition::BottomLeft));
    }


    UpdateStatus update(
        const GrapitoTimer &,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override
    {
        // TODO This should be handled for all active controllers, there might not even be a gamepad.
        if (aInputs.get(Controller::Gamepad_0)[Back].positiveEdge() && (mGameRule.mPlayers.size() > 1))
        {
            aStateMachine.putNext(getPhase(GameRule::Warmup));
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

    aunteater::weak_entity mHudText;
};


GameRule::PhasesArray setupGamePhases(std::shared_ptr<Context> aContext, GameRule & aGameRule)
{
    GameRule::PhasesArray result;
    result[GameRule::FreeSolo] = std::make_shared<FreeSoloPhase>(aContext, aGameRule);
    result[GameRule::Warmup] = std::make_shared<WarmupPhase>(aContext, aGameRule);
    result[GameRule::Competition] = std::make_shared<CompetitionPhase>(aContext, aGameRule);
    result[GameRule::Congratulation] = std::make_shared<CongratulationPhase>(aContext, aGameRule);
    result[GameRule::ExpectPlayers] = std::make_shared<ExpectPlayersPhase>(aContext, aGameRule);
    return result;
}


GameRule::GameRule(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::vector<aunteater::Entity> aPlayers,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    mEntityManager{aEntityManager},
    mCompetitors{aEntityManager},
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager},
    mPlayers{std::move(aPlayers)},
    mContext{std::move(aContext)},
    mPhases{setupGamePhases(mContext, *this)},
    mPhaseMachine{mPhases[ExpectPlayers]},
    mControlSystem{std::move(aControlSystem)},
    mRenderToScreenSystem{std::move(aRenderToScreenSystem)}
{}


void GameRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    mPhaseMachine.update(aTimer, aInput);
}


std::size_t GameRule::eliminateCompetitors()
{
    // There is the one camera
    Position2 cameraPosition = mCameras.begin()->get<Position>().position;

    std::size_t remainingCompetitors = 0;
    for (auto competitor : mCompetitors)
    {
        auto & [cameraGuide, playerData, geometry] = competitor;            
        if (geometry.position.y() < (cameraPosition.y() - game::gCompetitorEliminationDistance))
        {
            ADLOG(info)("Player {} eliminated!", playerData.id);

            prepareCameraFadeOut(cameraPosition, geometry, cameraGuide);

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
    return remainingCompetitors;
}


void GameRule::prepareCameraFadeOut(Position2 aCameraPosition,
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

        mEntityManager.addEntity(aunteater::Entity{}
            .add<CameraGuide>(smoothOut)
            .add<Position>(aGeometry)
        );
    }
}


void GameRule::resetCompetitors()
{
    killAllCompetitors();

    for (const PlayerControllerState & player : mContext->mPlayerList)
    {
        if (player.mJoinState == PlayerJoinState_Playing)
        {
            mEntityManager.addEntity(
                makePlayingPlayer(player.mPlayerSlot, player.mControllerId, player.mColor));
        }
    }
}


void GameRule::killAllCompetitors()
{
    for (auto competitor : mCompetitors)
    {
        kill(competitor);
    }
}


void GameRule::enableGrapples()
{
    mControlSystem->switchGrappling(true);
}


void GameRule::disableGrapples()
{
    mControlSystem->switchGrappling(false);
}


void GameRule::setFadeOpacity(float aOpacity)
{
    mRenderToScreenSystem->setFade({game::gFadeColor, (std::uint8_t)(aOpacity * 255)});
}


void GameRule::disableFade()
{
    mRenderToScreenSystem->disableFade();
}


} // namespace grapito
} // namespace ad
