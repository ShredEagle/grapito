#include "MenuScene.h"

#include "RopeGame.h"

#include "../commons.h"
#include "../Configuration.h"
#include "../Logging.h"

#include "../Utils/DrawDebugStuff.h"
#include "../Utils/MenuControls.h"

#include <math/VectorUtilities.h>

#include <graphics/CameraUtilities.h>


namespace ad {
namespace grapito {


auto makeInterpolation(GLfloat aStartFactor, GLfloat aEndFactor)
{
    GLfloat viewedWidth_w = math::getRatio<float>(game::gAppResolution) * menu::gViewedHeight;
    return math::makeInterpolation<math::None, math::ease::SmoothStep>(
        aStartFactor * viewedWidth_w,
        aEndFactor   * viewedWidth_w,
        menu::gTransitionDuration);
}

MenuScene::MenuScene(Menu aMenu,
                     const filesystem::path & aFontPath,
                     std::shared_ptr<graphics::AppInterface> aAppInterface,
                     std::shared_ptr<const GameScene> aGameScene,
                     std::function<void()> aEnterFunc,
                     std::function<void()> aExitFunc,
                     std::optional<arte::ImageRgba> aImage) :
    mMenu{std::move(aMenu)},
    mAppInterface{std::move(aAppInterface)},
    mOptionalGameScene{aGameScene},
    mOptionalEnter{aEnterFunc},
    mOptionalExit{aExitFunc},
    mRenderEffect{mAppInterface},
    mShaping{game::gAppResolution},
    mTexting{aFontPath, menu::gTextHeight, menu::gViewedHeight, mAppInterface},
    // Useless, it is setup before transitions. But there is no default ctor.
    mMenuXPosition{makeInterpolation(0.f, 0.f)},
    mImageBackground{aImage},
    mSpriting{}
{
    updateMenuBuffers();
}


UpdateStatus MenuScene::update(
        const GrapitoTimer & /*aTimer*/,
        const GameInputState & aInputs,
        StateMachine & aStateMachine)
{
    // Note: The logic eagerly re-creates the text buffers each time the selection change.
    // This is currently not necessary, but make the logic a bit simpler.

    const Controller pressStartController = isPositiveEdge(aInputs, Start);

    if (isPositiveEdge(aInputs, Up, LeftVerticalAxis, AxisSign::Positive) != Controller::_End)
    {
        mMenu.mSelected = std::min(mMenu.mSelected - 1, mMenu.size() - 1);
        updateMenuBuffers();
    }
    else if (isPositiveEdge(aInputs, Down, LeftVerticalAxis, AxisSign::Negative) != Controller::_End)
    {
        mMenu.mSelected = (mMenu.mSelected + 1) % mMenu.size();
        updateMenuBuffers();
    }
    else if (pressStartController != Controller::_End)
    {
        mMenu.selected().mCallback(aStateMachine, mAppInterface, pressStartController);
        // When the menu is poping itself, the OpenGL is issuing an error when return SwapBuffers:
        // GL_INVALID_VALUE error generated. Program handle does not refer to an object generated by OpenGL.
        // We avoid it by not swapping to cover this case.
        // TODO get a better understanding of the error scenario.
        return UpdateStatus::KeepFrame;
    }
    
    renderMenu();
    return UpdateStatus::SwapBuffers;
}


void MenuScene::beforeEnter()
{
    setViewedRectangle(
        mSpriting,
        math::Rectangle<GLfloat>{
            math::Position<2, GLfloat>::Zero(),
            static_cast<math::Size<2, GLfloat>>(game::gAppResolution)
    });
    mOptionalEnter();
    mMenuXPosition = makeInterpolation(0.5f, -0.5f);
}


void MenuScene::beforeExit()
{
    mOptionalExit();
    mMenuXPosition = makeInterpolation(-0.5f, -1.5f);
}


std::pair<TransitionProgress, UpdateStatus> MenuScene::scrollMenu(const GrapitoTimer & aTimer)
{
    Rectangle viewed = graphics::getViewRectangle(game::gAppResolution, menu::gViewedHeight);
    viewed.origin().x() = mMenuXPosition.advance(aTimer.delta());
    setViewedRectangle(mShaping, viewed);
    // Uncomment to also scroll the text
    //setViewedRectangle(mTexting, viewed);
    debugDrawer->setViewedRectangle(viewed);
    renderMenu();

    return {
        mMenuXPosition.isCompleted() ? TransitionProgress::Complete : TransitionProgress::Ongoing,
        UpdateStatus::SwapBuffers
    };
}


void MenuScene::updateMenuBuffers()
{
    std::vector<graphics::TrivialShaping::Rectangle> rectangles;
    graphics::Texting::Mapping strings;

    GLfloat menuHeight = mMenu.size() * menu::gButtonSize.height()
        + (mMenu.size() - 1) * menu::gButtonSpacing;
    GLfloat buttonY = menuHeight / 2.f - menu::gButtonSize.height() / 2.f;
    GLfloat incrementY = menu::gButtonSize.height() + menu::gButtonSpacing;

    for (std::size_t buttonId = 0; buttonId != mMenu.size(); ++buttonId)
    {
        math::Position<2, GLfloat> buttonCenter{0.f, buttonY};

        math::Rectangle<GLfloat> textBoundingBox =
            mTexting.getStringBounds(mMenu[buttonId].mText, { 0.f, 0.f });
        // The offset from the center of the bounding box to the initial pen position (origin)
        math::Vec<2, GLfloat> centeringPenOffset = -textBoundingBox.center().as<math::Vec>();
        mTexting.prepareString(mMenu[buttonId].mText, buttonCenter + centeringPenOffset, menu::gTextColor, strings);
        debugDrawer->drawOutline(
            mTexting.getStringBounds(mMenu[buttonId].mText, buttonCenter + centeringPenOffset));

        if (mMenu.mSelected == buttonId)
        {
            rectangles.push_back({
                Rectangle{ buttonCenter , menu::gButtonSize }.centered(),
                menu::gSelectedColor[buttonId % menu::gSelectedColor.size()]});
        }
        buttonY -= incrementY;
    }

    mShaping.updateInstances(rectangles);
    mTexting.updateInstances(strings);
}


void MenuScene::renderMenu()
{
    graphics::AppInterface::clear();

    if (mImageBackground)
    {
        if (!mLoadedBackground)
        {
            std::tie(mAtlas, mLoadedBackground) = graphics::sprite::load(*mImageBackground);
            mBackgroundPlacement = (- (*mImageBackground).dimensions() / 2).as<math::Position>();
        }
        mSpriting.updateInstances(
            std::array<graphics::Spriting::Instance, 1>{
            graphics::Spriting::Instance{
                {0.f, 0.f},
                *mLoadedBackground,
                1.f,
                graphics::Mirroring::FlipVertical}
        });
        mSpriting.render(mAtlas);
    }

    if (mOptionalGameScene)
    {
        mRenderEffect.blurTo(*mOptionalGameScene, graphics::FrameBuffer::Default(), menu::gBlurringPasses);
    }

    mShaping.render();
    mTexting.render();
}


} // namespace grapito
} // namespace ad