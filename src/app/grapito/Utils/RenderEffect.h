#pragma once


#include <graphics/AppInterface.h>

#include <graphics/effects/Fade.h>
#include <graphics/effects/GaussianBlur.h>


namespace ad {
namespace grapito {


class GameScene;


class RenderEffect
{
public:
    RenderEffect(std::shared_ptr<graphics::AppInterface> aAppInterface);

    void blurTo(const GameScene & aScene,
                graphics::FrameBuffer & aDestination,
                int aPassCount);

    void fadeTo(const GameScene & aScene,
                graphics::FrameBuffer & aDestination,
                math::sdr::Rgba aFadeToColor);

private:
    void renderGameScene(const GameScene & aScene);
    void onResize(math::Size<2, int> aNewSize);

    graphics::PingPongFrameBuffers mFrameBuffers;
    graphics::Fade mFade;
    graphics::GaussianBlur mGaussianBlur;
    std::shared_ptr<graphics::AppInterface::SizeListener> mSizeListener;
};


} // namespace grapito
} // namespace ad
