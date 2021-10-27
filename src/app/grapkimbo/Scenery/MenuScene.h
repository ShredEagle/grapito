#pragma once


#include "StateMachine.h"

#include <graphics/AppInterface.h>
#include <graphics/TrivialShaping.h>

#include <math/Interpolation.h>

#include <memory>
#include <vector>


namespace ad {
namespace grapito {


class GameScene;


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
    MenuScene(Menu aMenu,
              std::shared_ptr<graphics::AppInterface> aAppInterface,
              std::shared_ptr<GameScene> aGameScene = nullptr);

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
    // Keeping a shared_ptr to the game scene that is rendered below the menu has several advantages:
    // * it avoids having to dig in the StateMachine for the state of interest
    // * it ensures the game is kept alive at least until the menu is completely done (transitions included)
    // There is however a risk of circular keep-alive if the game later keeps a shared_ptr to the menu.
    std::shared_ptr<GameScene> mOptionalGameScene; 
    graphics::TrivialShaping mShaping;
    math::Interpolation<GLfloat, GrapitoTimer::Value_t, math::ease::SmoothStep<GrapitoTimer::Value_t>> mMenuXPosition;
}; 


} // namespace grapito
} // namespace ad
