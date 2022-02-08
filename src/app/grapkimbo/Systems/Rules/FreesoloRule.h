#pragma once

#include "RuleBase.h"

#include "../../Scenery/StateMachine.h"

#include <aunteater/EntityManager.h>
#include <aunteater/System.h>

#include <deque>


namespace ad {
namespace grapito {


class Control;
class RenderToScreen;


class FreesoloRule : public RuleBase
{

public:
    FreesoloRule(aunteater::EntityManager & aEntityManager,
                 std::shared_ptr<Context> aContext,
                 std::shared_ptr<Control>,
                 std::shared_ptr<RenderToScreen> aRenderToScreenSystem);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    aunteater::weak_entity mPlayer{nullptr};
    aunteater::weak_entity mHudText{nullptr};

    // We want size, not list init of the deque
    std::deque<int> mHeights = std::deque<int>(8, 0);
};


} // namespace grapito
} // namespace ad
