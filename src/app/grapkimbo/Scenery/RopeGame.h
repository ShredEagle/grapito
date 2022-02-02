#pragma once


#include "GameScene.h"

#include "../commons.h"
#include <utility>


namespace ad {
namespace grapito {

class RenderBackground;
class RenderWorld;

class RopeGame : public GameScene
{
public:
    RopeGame(std::shared_ptr<Context> aContext,
             std::shared_ptr<graphics::AppInterface> aAppInterface);

    std::pair<TransitionProgress, UpdateStatus> enter(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &) override;
    std::pair<TransitionProgress, UpdateStatus> exit(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &) override;

    void render() const override;

private:
    std::shared_ptr<RenderBackground> mRenderBackgroundSystem;
    std::shared_ptr<RenderWorld> mRenderWorldSystem;
    ALuint mBgMusicSource;
};


} // namespace grapito
} // namespace ad
