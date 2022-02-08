#pragma once


#include "GameScene.h"

#include "../commons.h"
#include <utility>


namespace ad {
namespace grapito {

class RenderBackground;
class RenderHud;
class RenderWorld;


enum class GameMode
{
    Multiplayer,
    Freesolo,
};


class RopeGame : public GameScene
{
public:
    RopeGame(std::shared_ptr<Context> aContext,
             std::shared_ptr<graphics::AppInterface> aAppInterface,
             GameMode aGameMode,
             const Controller aController);

    std::pair<TransitionProgress, UpdateStatus> enter(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &) override;
    std::pair<TransitionProgress, UpdateStatus> exit(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &) override;

    void render() const override;

protected:
    std::shared_ptr<MenuScene> getPauseMenu() override;

private:
    std::shared_ptr<RenderBackground> mRenderBackgroundSystem;
    std::shared_ptr<RenderHud> mRenderHudSystem;
    std::shared_ptr<RenderWorld> mRenderWorldSystem;
    ALuint mBgMusicSource;
    GameMode mGameMode;
};


} // namespace grapito
} // namespace ad
