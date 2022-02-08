#include "FloorIsLavaRule.h"

#include "Phases.h"

#include "../../Configuration.h"
#include "../../Context/Context.h"
#include "../../Entities.h"
#include "../../Logging.h"
#include "../../Timer.h"

#include "../../Components/ScreenPosition.h"
#include "../../Components/Text.h"

#include "../../Utils/Player.h"

#include <handy/StringId_Interning.h>


namespace ad {
namespace grapito {


const StringId hud_lavadeath_sid = handy::internalizeString("hud_lavadeath");


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
            // TODO message
            aStateMachine.putNext(getPhase(FloorIsLavaRule::FloorIsLava));
            aStateMachine.popState();
        }
        // Note: Has no effect, nested state machines update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void beforeExit() override
    {}
};



class FloorIsLavaPhase : public PhaseParent
{
    using PhaseParent::PhaseParent;

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & /*aInputs*/,
        StateMachine & aStateMachine) override
    {
        if (isGrounded(mRule.player()->get<PlayerData>()))
        {
            // TODO check for highscore
            aStateMachine.putNext(getPhase(FloorIsLavaRule::Death));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }
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
    mPhases{setupGamePhases(mContext, *this)},
    // ATTENTION the state machine is entering the first state directly, before FloorIsLavaRule is done cting.
    mPhaseMachine{mPhases[InitialInvicible]}
{
    addPlayer();
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



} // namespace grapito
} // namespace ad