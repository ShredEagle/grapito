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
    {
        // Render the scene to a texture
        auto renderTexture = mFrameBuffers.bindTargetFrameBuffer();
        auto viewport = mFrameBuffers.scopeViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        aScene.render();
        mFrameBuffers.swap();
    }
    mGaussianBlur.apply(aPassCount, mFrameBuffers, &aDestination);
}


void RenderEffect::onResize(math::Size<2, int> aNewSize)
{
    spdlog::get("grapito")->trace("Resizing render effect buffers to ({}, {}).", aNewSize.width(), aNewSize.height());
    mFrameBuffers = graphics::PingPongFrameBuffers{aNewSize, GL_LINEAR};
}


} // namespace grapito
} // namespace ad
