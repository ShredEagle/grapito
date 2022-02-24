#include "RenderBackground.h"

#include "../Configuration.h"
#include "../Timer.h"
#include "../Utils/Camera.h"

#include <arte/Image.h>

#include <graphics/SpriteLoading.h>
#include <graphics/CameraUtilities.h>



namespace ad {
namespace grapito {


RenderBackground::RenderBackground(aunteater::EntityManager & aEntityManager,
                                   std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mEntityManager{aEntityManager},
    mAppInterface{aAppInterface},
    mCameras{mEntityManager},
    mParallaxScroller{aAppInterface->getWindowSize()},
    mSpriting{render::gSpritePixelWorldSize }
{}


void RenderBackground::addTiledLayer(const filesystem::path & aImage, float aScrollFactor)
{
    arte::Image<math::sdr::Rgba> image{aImage, arte::ImageOrientation::InvertVerticalAxis};
    auto [atlas, loadedTile] = graphics::sprite::load(image);

    mParallaxScroller.addLayer(
        atlas, 
        image.dimensions(),
        [tile = std::move(loadedTile)](auto){return tile;},
        aScrollFactor);
}

void RenderBackground::addLayer(const filesystem::path& aImage, Vec3 aLayerWorldPos)
{
    arte::Image<math::sdr::Rgba> image{ aImage, arte::ImageOrientation::InvertVerticalAxis };
    graphics::sprite::SingleLoad load = graphics::sprite::load(image);

    mSingleLayerAtlases.emplace_back(load, aLayerWorldPos);
    mAtlasToSprite.emplace_back();
}


void RenderBackground::update(const GrapitoTimer, const GameInputState &)
{
    mParallaxScroller.positionCamera(getCameraPosition(mCameras));

    //Set viewed rectangle
    auto position = getCameraPosition(mCameras);
    math::Rectangle<GLfloat> viewed = getViewedRectangle(position, math::getRatio<GLfloat>(mAppInterface->getWindowSize()));
    setViewedRectangle(mSpriting, viewed);

    std::ranges::for_each(mAtlasToSprite, [](auto& aSprites) {aSprites.clear(); });
    //for all single layer compute their 2D pos based on their 3D pos
    for (AtlasIndex atlas = 0; atlas != mSingleLayerAtlases.size(); ++atlas)
    {
        Position3 pos3D = mSingleLayerAtlases.at(atlas).second;
        Position2 pos = {pos3D.x() + position.x() * pos3D.z() * render::gSpritePixelWorldSize, pos3D.y() + position.y() * pos3D.z() * render::gSpritePixelWorldSize};
        mAtlasToSprite.at(atlas).emplace_back(
                pos,
                mSingleLayerAtlases.at(atlas).first.second,
                render::gSpriteOpacity
        );
    }
}


void RenderBackground::render() const
{
    mAppInterface->clear();
    mParallaxScroller.render();
    for (AtlasIndex atlas = 0; atlas != mSingleLayerAtlases.size(); ++atlas)
    {
        mSpriting.updateInstances(mAtlasToSprite.at(atlas));
        mSpriting.render(mSingleLayerAtlases.at(atlas).first.first);
    }
}


} // namespace grapito
} // namespace ad
