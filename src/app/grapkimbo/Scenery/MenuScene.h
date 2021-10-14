#pragma once


#include "StateMachine.h"

#include <graphics/AppInterface.h>
#include <graphics/TrivialShaping.h>

#include <memory>
#include <vector>


namespace ad {
namespace grapito {


struct UiButton
{
    //GLfloat mYPosition;
    std::function<void(StateMachine &, std::shared_ptr<graphics::AppInterface> &)> mCallback =
        [](StateMachine &, std::shared_ptr<graphics::AppInterface> &){};
};


struct Menu
{
    UiButton & selected()
    { return mButtons[mSelected]; }

    auto size () const
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

private:
    Menu mMenu;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    graphics::TrivialShaping mShaping;
}; 


} // namespace grapito
} // namespace ad
