#include "SplashScene.h"

#include "../Configuration.h"

#include <graphics/CameraUtilities.h>

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito {


math::Size<2, int> getScreenSizeInWorld(math::Size<2, int> aBufferResolution)
{
    return math::makeSizeFromHeight(splash::gVerticalResolution,    
                                    math::getRatio<float>(aBufferResolution));
}


SplashScene::SplashScene(math::Size<2, int> aResolution) :
    mScreenSize_w{getScreenSizeInWorld(aResolution)},
    mSpriting{}
{
    setViewedRectangle(
        mSpriting,
        math::Rectangle<GLfloat>{
            math::Position<2, GLfloat>::Zero(), 
            static_cast<math::Size<2, GLfloat>>(mScreenSize_w)
        });
}


UpdateStatus SplashScene::update(
    const GrapitoTimer & aTimer,
    const GameInputState & aInputs,
    StateMachine & aStateMachine)
{
    mTimeAccumlated += aTimer.delta();
    // If accumulated time gets longer than current splash duration,
    // advance to next splash.
    while (mCurrentSplashId < splashes.size() 
        && mTimeAccumlated > current().mDuration)
    {
        mTimeAccumlated -= current().mDuration;
        mLoadedSprite.reset();
        if (++mCurrentSplashId == splashes.size())
        {
            // All splashes have completed
            reset(); // in case this scene is later re-entered.
            // when done, pop this state to execute next state in the stack
            // (e.g. a main menu)
            aStateMachine.popState();
            return UpdateStatus::KeepFrame;
        }
    }

    if (! mLoadedSprite)
    {
        mLoadedSprite = mSpriting.load(current().mImage);
        mSpritePlacement_w = ((mScreenSize_w - current().mImage.dimensions()) / 2).as<math::Position>();
    }

    graphics::AppInterface::setClearColor(current().mBufferClearColor(aTimer.delta()));
    graphics::AppInterface::clear();
    mSpriting.updateInstances(
        std::array<graphics::Spriting::Instance, 1>{ 
            graphics::Spriting::Instance{
                static_cast<math::Position<2, GLfloat>>(mSpritePlacement_w),
                *mLoadedSprite,
                current().mSpriteOpacity(aTimer.delta())} 
        });
    mSpriting.render();
    return UpdateStatus::SwapBuffers;
}


Splash & SplashScene::current()
{
    return splashes.at(mCurrentSplashId);
}


void SplashScene::reset()
{
    mCurrentSplashId = 0;
    mTimeAccumlated = 0.;
    mLoadedSprite.reset();
}


void SplashScene::onFramebufferResize(math::Size<2, int> aResolution)
{
    // The size of the screen in world units (pixels)
    mScreenSize_w = getScreenSizeInWorld(aResolution);
}


} // namespace grapito
} // namespace ad
