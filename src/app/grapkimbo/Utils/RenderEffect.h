#pragma once


#include <graphics/AppInterface.h>

#include <graphics/effects/GaussianBlur.h>


namespace ad {
namespace grapito {


class GameScene;


class RenderEffect
{
public:
    RenderEffect(std::shared_ptr<graphics::AppInterface> aAppInterface);

    void blurTo(const GameScene & aScene, graphics::FrameBuffer & aDestination, int aPassCount);

private:
    void onResize(math::Size<2, int> aNewSize);

    graphics::PingPongFrameBuffers mFrameBuffers;
    graphics::GaussianBlur mGaussianBlur;
    std::shared_ptr<graphics::AppInterface::SizeListener> mSizeListener;
};


} // namespace grapito
} // namespace ad
