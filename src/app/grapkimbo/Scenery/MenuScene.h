#pragma once


#include "StateMachine.h"

#include <graphics/AppInterface.h>
#include <graphics/TrivialShaping.h>

#include <math/Interpolation.h>

#include <memory>
#include <vector>


namespace ad {
namespace grapito {


struct UiButton
{
    const std::string mText;
    std::function<void(StateMachine &, std::shared_ptr<graphics::AppInterface> &)> mCallback =
        [](StateMachine &, std::shared_ptr<graphics::AppInterface> &){};
};


struct Menu
{
    UiButton & selected()
    { return mButtons[mSelected]; }

    auto size() const
    { return mButtons.size(); }

    std::vector<UiButton> mButtons;
    std::vector<UiButton>::size_type mSelected = 0;
};


Menu makePauseMenu();


class MenuScene : public State
{
public:
    MenuScene(Menu aMenu, std::shared_ptr<graphics::AppInterface> aAppInterface);

    UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

    std::pair<TransitionProgress, UpdateStatus> enter(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        const StateMachine & aStateMachine) override
    { return scrollMenu(aTimer); }

    std::pair<TransitionProgress, UpdateStatus> exit(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        const StateMachine & aStateMachine) override
    { return scrollMenu(aTimer); }

    void beforeEnter() override;
    void beforeExit() override;

private:
    std::pair<TransitionProgress, UpdateStatus> MenuScene::scrollMenu(GrapitoTimer & aTimer);
    void renderMenu();

    Menu mMenu;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    graphics::TrivialShaping mShaping;
    math::Interpolation<GLfloat, GrapitoTimer::Value_t, math::ease::SmoothStep<GrapitoTimer::Value_t>> mMenuXPosition;
}; 


} // namespace grapito
} // namespace ad
