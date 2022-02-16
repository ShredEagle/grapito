#pragma once

#include "../commons.h"
#include "../Input.h"

#include "../Utils/RenderEffect.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito
{


class GameScene;


class RenderToScreen : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    RenderToScreen(aunteater::EntityManager & aEntityManager,
                   std::shared_ptr<graphics::AppInterface> aAppInterface,
                   const GameScene & aGameScene);

    // Does the actual rendering and show it on screen
    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    void setFade(math::sdr::Rgba aFadeToColor)
    {
        mFadeToColor = aFadeToColor;
    }

    void disableFade()
    {
        mFadeToColor = std::nullopt;
    }

private:
    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    const GameScene & mRenderedScene;
    RenderEffect mEffects;

    std::optional<math::sdr::Rgba> mFadeToColor;
};


} // namespace grapito
} // namespace ad
