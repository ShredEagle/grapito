#pragma once

#include "../Context/Context.h"

#include "CameraGuidedControl.h" // for Camera archetype

#include <graphics/adapters/ParallaxScroller.h>
#include <graphics/Spriting.h>


namespace ad {
namespace grapito
{

//typedef aunteater::Archetype<Background> RenderedBackground;

class RenderBackground : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    RenderBackground(aunteater::EntityManager & aEntityManager,
                     std::shared_ptr<graphics::AppInterface> aAppInterface);

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    void render() const;

    void addTiledLayer(const filesystem::path & aImage, float aScrollFactor);

    void addLayer(const filesystem::path& aImage, Vec3 aLayerPos);

private:

    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<graphics::AppInterface> mAppInterface;
    const aunteater::FamilyHelp<Camera> mCameras;

    graphics::ParallaxScroller mParallaxScroller;
    mutable graphics::Spriting mSpriting;
    std::vector<std::vector<graphics::Spriting::Instance>> mAtlasToSprite;
    std::vector<std::pair<graphics::sprite::SingleLoad, Position3>> mSingleLayerAtlases;
};


} // namespace grapito
} // namespace ad
