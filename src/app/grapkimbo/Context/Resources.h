#pragma once


#include "../Ressources/SoundManager.h"

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
                              loadOggFileFromPath> oggSoundFiles;
};

} // namespace grapito
} // namespace ad
