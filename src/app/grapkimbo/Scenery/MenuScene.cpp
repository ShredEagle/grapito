#include "MenuScene.h"

#include "../commons.h"
#include "../Configuration.h"

#include <graphics/CameraUtilities.h>


namespace ad {
namespace grapito {


MenuScene::MenuScene(std::shared_ptr<graphics::AppInterface> & aAppInterface) :
    mShaping{aAppInterface->getFramebufferSize()}
{
    setViewedRectangle(
        mShaping,
        graphics::getViewRectangle(aAppInterface->getFramebufferSize(), menu::gViewedHeight));
    mShaping.addRectangle({
        Rectangle{ {0.f, -menu::gButtonSpacing}, menu::gButtonSize }.centered(),
        menu::gButtonColor });
    mShaping.addRectangle({
        Rectangle{ {0.f,                   0.f}, menu::gButtonSize }.centered(),
        menu::gButtonColor });
    mShaping.addRectangle({
        Rectangle{ {0.f,  menu::gButtonSpacing}, menu::gButtonSize }.centered(),
        menu::gButtonColor });
}


UpdateStatus MenuScene::update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine)
{
    graphics::AppInterface::clear();

    InputState gamePause = aInputs.get(Controller::Keyboard)[Command::GamePause];
    if (gamePause.positiveEdge())
    {
        auto removed = aStateMachine.popState();
        return UpdateStatus::KeepFrame;
    }
    else
    {
        mShaping.render();
        return UpdateStatus::SwapBuffers;
    }

}


} // namespace grapito
} // namespace ad
