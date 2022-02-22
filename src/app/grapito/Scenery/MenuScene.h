#pragma once


#include "StateMachine.h"

#include "../Timer.h"
#include "../Utils/RenderEffect.h"

#include <graphics/AppInterface.h>
#include <graphics/SpriteLoading.h>
#include <graphics/Spriting.h>
#include <graphics/Texting.h>
#include <graphics/TrivialShaping.h>

#include <math/Interpolation/Interpolation.h>

#include <memory>
#include <vector>


namespace ad {
namespace grapito {


class GameScene;


struct UiButton
{
    const std::string mText;
    std::function<void(StateMachine &, std::shared_ptr<graphics::AppInterface> &, const Controller)> mCallback =
        [](StateMachine &, std::shared_ptr<graphics::AppInterface> &, const Controller){};
};


struct Menu
{
    UiButton & selected()
    { return mButtons[mSelected]; }

    auto size() const
    { return mButtons.size(); }

    UiButton & operator[](std::size_t aButtonIndex)
    {
        return mButtons[aButtonIndex];
    }

    std::vector<UiButton> mButtons;
    std::vector<UiButton>::size_type mSelected = 0;
};


Menu makePauseMenu();


class MenuScene : public State
{
public:
    MenuScene(Menu aMenu,
              const filesystem::path & aFontPath,
              std::shared_ptr<graphics::AppInterface> aAppInterface,
              std::shared_ptr<const GameScene> aGameScene = nullptr,
              std::function<void ()> aEnterFunc = []() {},
              std::function<void ()> aExitFunc = []() {},
              std::optional<arte::ImageRgba> aImage = std::nullopt);

    UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

    std::pair<TransitionProgress, UpdateStatus> enter(
        const GrapitoTimer & aTimer,
        const GameInputState &,
        const StateMachine &) override
    { return scrollMenu(aTimer); }

    std::pair<TransitionProgress, UpdateStatus> exit(
        const GrapitoTimer & aTimer,
        const GameInputState &,
        const StateMachine &) override
    { return scrollMenu(aTimer); }

    void beforeEnter() override;
    void beforeExit() override;

private:
    std::pair<TransitionProgress, UpdateStatus> scrollMenu(const GrapitoTimer & aTimer);

    void prepareVersion(graphics::Texting::Mapping & aStrings);

    /// \brief Generate menu geometry and fonts, and update all vertex buffers of renderers.
    void updateMenuBuffers();

    /// \brief Have renderers draw the menu to screen.
    void renderMenu();

    Menu mMenu;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    // Keeping a shared_ptr to the game scene that is rendered below the menu has several advantages:
    // * it avoids having to dig in the StateMachine for the state of interest
    // * it ensures the game is kept alive at least until the menu is completely done (transitions included)
    // There is however a risk of circular keep-alive if the game later keeps a shared_ptr to the menu.
    std::shared_ptr<const GameScene> mOptionalGameScene;
    std::function<void()> mOptionalEnter;
    std::function<void()> mOptionalExit;
    RenderEffect mRenderEffect;
    graphics::TrivialShaping mShaping;
    graphics::Texting mTexting;
    math::Interpolation<
        GLfloat,
        GrapitoTimer::Value_t,
        math::None,
        math::ease::SmoothStep> mMenuXPosition;
    graphics::sprite::LoadedAtlas mAtlas;
    graphics::Spriting mSpriting;
}; 


} // namespace grapito
} // namespace ad
