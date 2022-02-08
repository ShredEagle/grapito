#pragma once


#include "../Scenery/StateMachine.h"

#include <aunteater/EntityManager.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {


class Control;
class RenderToScreen;


class FreesoloRule : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    FreesoloRule(aunteater::EntityManager & aEntityManager,
                 std::shared_ptr<Context> aContext,
                 std::shared_ptr<Control>,
                 std::shared_ptr<RenderToScreen> aRenderToScreenSystem);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    aunteater::EntityManager & mEntityManager;

    std::shared_ptr<Control> mControlSystem;
    std::shared_ptr<RenderToScreen> mRenderToScreenSystem;

    std::shared_ptr<Context> mContext;
};


} // namespace grapito
} // namespace ad
