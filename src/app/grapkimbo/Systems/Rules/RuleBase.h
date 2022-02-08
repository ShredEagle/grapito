#pragma once


#include "../../Scenery/StateMachine.h"

#include <aunteater/EntityManager.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {


class Control;
class RenderToScreen;


template <class T_rule>
class PhaseBase : public State
{
public:
    PhaseBase(std::shared_ptr<Context> aContext, T_rule & aRule) :
        mContext{std::move(aContext)},
        mRule{aRule}
    {}

protected:
    std::shared_ptr<State> getPhase(T_rule::Phase aPhase)
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



class RuleBase : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    RuleBase(aunteater::EntityManager & aEntityManager,
             std::shared_ptr<Context> aContext,
             std::shared_ptr<Control> aControlSystem,
             std::shared_ptr<RenderToScreen> aRenderToScreenSystem);

protected:
    void enableGrapples();
    void disableGrapples();

    void setFadeOpacity(float aOpacity);
    void disableFade();

    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<Context> mContext;

    std::shared_ptr<Control> mControlSystem;
    std::shared_ptr<RenderToScreen> mRenderToScreenSystem;
};


} // namespace grapito
} // namespace ad
