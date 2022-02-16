#pragma once


#include "PlayerList.h"
#include "Localization.h"
#include "Resources.h"
#include "commons.h"


namespace ad {
namespace grapito {


/// \brief Intended to be passed around in different places of the code.
/// It centralizes different objects that would otherwise easily become globals.
struct Context
{
    Context(const filesystem::path aRoot);

    /// \brief Get the localized string for `aStringId`, in the active language of `locale` data member.
    const std::string & translate(StringId aStringId) const;

    filesystem::path pathFor(const filesystem::path &aAsset) const;

    const arte::AnimationSpriteSheet & loadAnimationSpriteSheet(const filesystem::path &aAsset);
    const graphics::sprite::SingleLoad & loadSingleSprite(const filesystem::path &aAsset);
    const OggSoundData & loadOggSoundData(const filesystem::path &aAsset, bool streamed);

    Resources resources;
    Locale locale;
    SoundManager mSoundManager;

    PlayerList mPlayerList;
};


//
// Implementations
//
inline Context::Context(const filesystem::path aRoot) :
    resources{{std::move(aRoot)}},
    locale{resources.locator.pathFor("localization/strings.json")}
{}


inline const std::string & Context::translate(StringId aStringId) const
{
    return locale.translate(aStringId);
}


inline filesystem::path Context::pathFor(const filesystem::path &aAsset) const
{
    return resources.locator.pathFor(aAsset);
}


inline const arte::AnimationSpriteSheet & Context::loadAnimationSpriteSheet(const filesystem::path &aAsset)
{
    return resources.animationSpriteSheets.load(aAsset, resources.locator); 
}


inline const graphics::sprite::SingleLoad & Context::loadSingleSprite(const filesystem::path &aAsset)
{
    return resources.singleSprites.load(aAsset, resources.locator); 
}


inline const OggSoundData & Context::loadOggSoundData(const filesystem::path &aAsset, bool streamed)
{
    const OggSoundData & data = resources.oggSoundFiles.load(aAsset, resources.locator, streamed);
    mSoundManager.storeDataInLoadedSound(data);
    return data;
}


} // namespace grapito
} // namespace ad
