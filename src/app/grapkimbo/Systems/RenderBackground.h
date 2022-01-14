#pragma once

#include "../Context/Context.h"

#include "CameraGuidedControl.h" // for Camera archetype

#include <graphics/adapters/ParallaxScroller.h>


namespace ad {
namespace grapito
{

//typedef aunteater::Archetype<Background> RenderedBackground;

class RenderBackground : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    RenderBackground(aunteater::EntityManager & aEntityManager,
                     std::shared_ptr<graphics::AppInterface> aAppInterface);

    void render() const;

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    void addLayer(const filesystem::path & aImage, float aScrollFactor);

private:

    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    const aunteater::FamilyHelp<Camera> mCameras;

    graphics::ParallaxScroller mParallaxScroller;

};


} // namespace grapito
} // namespace ad
