#pragma once


#include "commons.h"
#include "sounds/SoundManager.h"
#include <arte/SpriteSheet.h>

#include <graphics/SpriteLoading.h>

#include <resource/ResourceLocator.h>
#include <resource/ResourceManager.h>


namespace ad {
namespace grapito {


struct Resources
{
    resource::ResourceLocator locator;
    resource::ResourceManager<arte::AnimationSpriteSheet, 
                              &arte::AnimationSpriteSheet::LoadAseFile> animationSpriteSheets{};
    resource::ResourceManager<graphics::sprite::SingleLoad, 
                              [](const filesystem::path & aImagePath) -> graphics::sprite::SingleLoad
                                {
                                    return graphics::sprite::load(arte::ImageRgba{aImagePath});
                                }> singleSprites{};
    resource::ResourceManager<OggSoundData,
                              sounds::loadOggFileFromPath> oggSoundFiles{};
};

} // namespace grapito
} // namespace ad
