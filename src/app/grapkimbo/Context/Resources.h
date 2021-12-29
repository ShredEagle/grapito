#pragma once


#include <arte/SpriteSheet.h>

#include <resource/ResourceLocator.h>
#include <resource/ResourceManager.h>


namespace ad {
namespace grapito {


struct Resources
{
    resource::ResourceLocator locator;
    resource::ResourceManager<arte::AnimationSpriteSheet, 
                              &arte::AnimationSpriteSheet::LoadAseFile> animationSpriteSheets;
    resource::ResourceManager<OggSoundData, 
                              OggSoundData::LoadOggFile> animationSpriteSheets;
};

} // namespace grapito
} // namespace ad
