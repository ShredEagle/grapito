#pragma once


#include "../../Configuration.h"
#include "../../Entities.h"
#include "../../Timer.h"

#include "../../Components/ScreenPosition.h"

#include "../../Scenery/StateMachine.h"

#include "../../Utils/CompositeTransition.h"


#include <aunteater/EntityManager.h>


namespace ad {
namespace grapito {


template <class T_rule>
class PhaseBase : public State
{
public:
    PhaseBase(std::shared_ptr<Context> aContext, T_rule & aRule) :
        mContext{std::move(aContext)},
        mRule{aRule}
    {}

protected:
    std::shared_ptr<State> getPhase(typename T_rule::Phase aPhase)
    {
        return mRule.mPhases[aPhase];
    }

    aunteater::EntityManager & getEntityManager()
    {
        return mRule.mEntityManager;
    }

    std::shared_ptr<Context> mContext;
    T_rule & mRule;
};


template <class T_rule>
class MessagePhase : public PhaseBase<T_rule>
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
    MessagePhase(std::shared_ptr<Context> aContext, T_rule & aCompetitionRule,
                 std::string aMessage, typename T_rule::Phase aNextPhase) :
        PhaseBase<T_rule>{std::move(aContext), aCompetitionRule},
        mMessage{std::move(aMessage)},
        mNextPhase{aNextPhase},
        mHudPositionInterpolationReference{PrepareInterpolation()},
        mHudPositionInterpolation{mHudPositionInterpolationReference}
    {}


    void beforeEnter() override
    {
        resetInterpolation();
        mHudText = this->getEntityManager().addEntity(makeHudText(mMessage,
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
            aStateMachine.putNext(this->getPhase(mNextPhase));
            aStateMachine.popState();
        }

        // Note: This will have absolutely no impact, nested state machines
        // update status is not checked.
        return UpdateStatus::SwapBuffers;
    }

    void beforeExit() override
    {
        // TODO delay delete ?
        mHudText->markToRemove();
    }


private:
    void resetInterpolation()
    {
        mHudPositionInterpolation = mHudPositionInterpolationReference;
    }

    std::string mMessage; 
    typename T_rule::Phase mNextPhase;

    const CompositeTransition<Position2, float> mHudPositionInterpolationReference;
    CompositeTransition<Position2, float> mHudPositionInterpolation;
    aunteater::weak_entity mHudText{nullptr};
};


} // namespace grapito
} // namespace ad
