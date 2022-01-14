#include "RenderBackground.h"

#include "../Configuration.h"
#include "../Timer.h"

#include <arte/Image.h>

#include <graphics/SpriteLoading.h>


namespace ad {
namespace grapito {


RenderBackground::RenderBackground(aunteater::EntityManager & aEntityManager,
                                   std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mEntityManager{aEntityManager},
    mAppInterface{aAppInterface},
    mCameras{mEntityManager},
    mParallaxScroller{aAppInterface->getWindowSize()}
{}


void RenderBackground::addLayer(const filesystem::path & aImage, float aScrollFactor)
{
    arte::Image<math::sdr::Rgba> image{aImage, arte::ImageOrientation::InvertVerticalAxis};
    auto [atlas, loadedTiles] = graphics::sprites::load(image);

    mParallaxScroller.addLayer(
        atlas, 
        image.dimensions(),
        [tiles = std::move(loadedTiles)](auto){return tiles.at(0);},
        aScrollFactor);
}


void RenderBackground::update(const GrapitoTimer, const GameInputState &)
{
    mParallaxScroller.positionCamera(getCameraPosition(mCameras));
    render();
}


void RenderBackground::render() const
{
    mAppInterface->clear();
    mParallaxScroller.render();
}


} // namespace grapito
} // namespace ad
