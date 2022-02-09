#pragma once


#include "../../Scenery/StateMachine.h"

#include <aunteater/EntityManager.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {


class Control;
class RenderToScreen;


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
