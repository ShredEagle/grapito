#include "RuleBase.h"

#include "../Control.h"
#include "../RenderToScreen.h"


namespace ad {
namespace grapito {


RuleBase::RuleBase(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<Context> aContext,
                   std::shared_ptr<Control> aControlSystem,
                   std::shared_ptr<RenderToScreen> aRenderToScreenSystem) :
    mEntityManager{aEntityManager},
    mContext{std::move(aContext)},
    mControlSystem{std::move(aControlSystem)},
    mRenderToScreenSystem{std::move(aRenderToScreenSystem)}
{}


void RuleBase::enableGrapples()
{
    mControlSystem->switchGrappling(true);
}


void RuleBase::disableGrapples()
{
    mControlSystem->switchGrappling(false);
}


void RuleBase::setFadeOpacity(float aOpacity)
{
    mRenderToScreenSystem->setFade({game::gFadeColor, (std::uint8_t)(aOpacity * 255)});
}


void RuleBase::disableFade()
{
    mRenderToScreenSystem->disableFade();
}


} // namespace grapito
} // namespace ad
