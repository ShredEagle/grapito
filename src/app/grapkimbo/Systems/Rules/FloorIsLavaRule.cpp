#include "FloorIsLavaRule.h"

#include "Phases.h"

#include "../../Configuration.h"
#include "../../Context/Context.h"
#include "../../Entities.h"
#include "../../Logging.h"
#include "../../Timer.h"

#include "../../Components/ScreenPosition.h"
#include "../../Components/SoundPlayer.h"
#include "../../Components/Text.h"

#include "../../Utils/Player.h"

#include <handy/StringId_Interning.h>


namespace ad {
namespace grapito {


const StringId hud_lavadeath_sid        = handy::internalizeString("hud_lavadeath");
const StringId hud_lavainstruction_sid  = handy::internalizeString("hud_lavainstruction");
const StringId hud_record_sid           = handy::internalizeString("hud_record");

const StringId soundId_BurnSid = handy::internalizeString("burn");


using PhaseParent = PhaseBase<FloorIsLavaRule>;


using DeathPhase = MessagePhase<FloorIsLavaRule>;


class LavaFadeOutPhase : public PhaseParent
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
            aStateMachine.putNext(getPhase(FloorIsLavaRule::FadeIn));
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
class LavaFadeInPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {
        mRule.setHeight(0);
        mRule.enableGrapples();
        mFadeOpacity.reset();
        mRule.resetPlayer();
    }


    UpdateStatus update(
            const GrapitoTimer & aTimer,
            const GameInputState &,
            StateMachine & aStateMachine) override
    {
        mRule.setFadeOpacity(mFadeOpacity.advance(aTimer.delta()));
        if(mFadeOpacity.isCompleted())
        {
            aStateMachine.putNext(getPhase(FloorIsLavaRule::InitialInvicible));
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
class InitialInviciblePhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    void beforeEnter() override
    {}

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override
    {
        if (mRule.player()->get<Position>().position.y() > game::FloorBecomesLavaHeight)
        {
            aStateMachine.putNext(getPhase(FloorIsLavaRule::FloorIsLava));
            aStateMachine.popState();
        }
        // Note: Has no effect, nested state machines update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void beforeExit() override
    {}
};


struct OpacityInterpolation
{
    static auto makeInterpolation(float aDuration)
    {
        CompositeTransition<float, float> result{0.f};
        result
            .pushInterpolation<math::None>(1.f, aDuration * (1./3.))
            .pushConstant(aDuration * (1./3.))
            .pushInterpolation<math::None>(0.f, aDuration * (1./3.))
        ;
        return result;
    }

    OpacityInterpolation(float aDuration) :
        mInterpolationReference{makeInterpolation(aDuration)},
        mInterpolation{mInterpolationReference}
    {}

    void reset()
    {
        mInterpolation = mInterpolationReference;
    }

    const CompositeTransition<float, float> mInterpolationReference;
    CompositeTransition<float, float> mInterpolation;
};


class FloorIsLavaPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    // Should be a beforeEnter, but needs the timer
    void beforeEnter() override
    {
        mOpacity.reset();
        mMaxHeight = 0;
        mHudInstructionText = 
            getEntityManager().addEntity(makeHudText(mContext->translate(hud_lavainstruction_sid),
                                                     hud::gCountdownPosition,
                                                     ScreenPosition::Center));
    }

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & /*aInputs*/,
        StateMachine & aStateMachine) override
    {
        if (mHudInstructionText)
        {
            (*mHudInstructionText)->get<Text>().color = 
                math::sdr::Rgba{hud::gTextColor, (std::uint8_t)(255 * mOpacity.mInterpolation.advance(aTimer.delta()))};
            if (mOpacity.mInterpolation.isCompleted())
            {
                (*mHudInstructionText)->markToRemove();
                mHudInstructionText = std::nullopt;
            }
        }

        mMaxHeight = std::max((int)std::floor(mRule.player()->get<Position>().position.y()), mMaxHeight);
        mRule.setHeight(mMaxHeight);

        if (isGrounded(mRule.player()->get<PlayerData>()))
        {
            // TODO check for highscore
            mRule.disableGrapples();
            addSoundToEntity(mRule.player(), soundId_BurnSid);
            aStateMachine.putNext(getPhase(FloorIsLavaRule::Death));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void beforeExit() override
    {
        // Just in case the removal condition was not already met before exiting
        if (mHudInstructionText)
        {
            (*mHudInstructionText)->markToRemove();
        }
    }

    std::optional<aunteater::weak_entity> mHudInstructionText;
    int mMaxHeight;
    OpacityInterpolation mOpacity{hud::gInstructionMessageDuration}; 
};


FloorIsLavaRule::PhasesArray setupGamePhases(std::shared_ptr<Context> aContext, FloorIsLavaRule & aFloorIsLavaRule)
{
    FloorIsLavaRule::PhasesArray result;
    result[FloorIsLavaRule::InitialInvicible] = std::make_shared<InitialInviciblePhase>(aContext, aFloorIsLavaRule);
    result[FloorIsLavaRule::FloorIsLava] = std::make_shared<FloorIsLavaPhase>(aContext, aFloorIsLavaRule);
    result[FloorIsLavaRule::Death] = 
        std::make_shared<DeathPhase>(aContext, aFloorIsLavaRule,
                                     aContext->translate(hud_lavadeath_sid), FloorIsLavaRule::FadeOut);
    result[FloorIsLavaRule::FadeOut] = std::make_shared<LavaFadeOutPhase>(aContext, aFloorIsLavaRule);
    result[FloorIsLavaRule::FadeIn] = std::make_shared<LavaFadeInPhase>(aContext, aFloorIsLavaRule);
    return result;
}


FloorIsLavaRule::FloorIsLavaRule(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    RuleBase{aEntityManager, std::move(aContext), std::move(aControlSystem), std::move(aRenderToScreenSystem)},
    mHudHeightText{mEntityManager.addEntity(makeHudText("",
                                                        hud::gAltimeterPosition,
                                                        ScreenPosition::Center))},
    mHudBestScoreText{mEntityManager.addEntity(makeHudText("",
                                                           hud::gBestScorePosition,
                                                           ScreenPosition::Center))},
    mPhases{setupGamePhases(mContext, *this)},
    // ATTENTION the state machine is entering the first state directly, before FloorIsLavaRule is done cting.
    mPhaseMachine{mPhases[InitialInvicible]}
{
    addPlayer();
    mHudHeightText->get<Text>().size = Text::Small;
    mHudBestScoreText->get<Text>().size = Text::Small;
}


void FloorIsLavaRule::update(const GrapitoTimer aTimer, const GameInputState & aInput)
{
    mPhaseMachine.update(aTimer, aInput);
}


void FloorIsLavaRule::addPlayer()
{
    PlayerControllerState player = *mContext->mPlayerList.begin();
    mPlayer = mEntityManager.addEntity(
        makePlayingPlayer(player.mPlayerSlot, player.mControllerId, player.mColor, {0.f, 3.f}));
}


void FloorIsLavaRule::resetPlayer()
{
    kill(player());
    addPlayer();
}


void FloorIsLavaRule::setHeight(int aHeight)
{
    mHudHeightText->get<Text>().message = std::to_string(aHeight);

    mBestHeight = std::max(aHeight, mBestHeight);
    mHudBestScoreText->get<Text>().message = 
        mContext->translate(hud_record_sid) + std::to_string(mBestHeight);
}


} // namespace grapito
} // namespace ad