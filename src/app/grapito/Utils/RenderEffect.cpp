#include "RenderEffect.h"

#include "../Scenery/GameScene.h"

#include "../Logging.h"


namespace ad {
namespace grapito {


using namespace std::placeholders;


RenderEffect::RenderEffect(std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mFrameBuffers{aAppInterface->getFramebufferSize(), GL_LINEAR},
    mSizeListener{aAppInterface->listenFramebufferResize(std::bind(&RenderEffect::onResize, this, _1))}
{}


void RenderEffect::blurTo(const GameScene & aScene, graphics::FrameBuffer & aDestination, int aPassCount)
{
    renderGameScene(aScene);
    mGaussianBlur.apply(aPassCount, mFrameBuffers, &aDestination);
}


void RenderEffect::fadeTo(const GameScene & aScene,
                          graphics::FrameBuffer & aDestination,
                          math::sdr::Rgba aFadeToColor)
{
    renderGameScene(aScene);
    mFade.apply(aFadeToColor, mFrameBuffers.getTexture(graphics::Role::Source), aDestination);
}


void RenderEffect::renderGameScene(const GameScene & aScene)
{
    // Render the game scene to the target texture in ping pong FBs.
    auto renderTexture = mFrameBuffers.bindTargetFrameBuffer();
    auto viewport = mFrameBuffers.scopeViewport();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    aScene.render();
    // The target texture becomes the source.
    mFrameBuffers.swap();
}


void RenderEffect::onResize(math::Size<2, int> aNewSize)
{
    spdlog::get("grapito")->trace("Resizing render effect buffers to ({}, {}).", aNewSize.width(), aNewSize.height());
    mFrameBuffers = graphics::PingPongFrameBuffers{aNewSize, GL_LINEAR};
}


} // namespace grapito
} // namespace ad
