#pragma once


#include "GameScene.h"


namespace ad {
namespace grapito {

class Render;

class RopeGame : public GameScene
{
public:
    RopeGame(std::shared_ptr<resource::ResourceManager> aResources,
             std::shared_ptr<graphics::AppInterface> aAppInterface);

    void render() const override;

private:
    std::shared_ptr<Render> mRenderSystem;
};


} // namespace grapito
} // namespace ad
