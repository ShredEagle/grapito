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
    Context(const filesystem::path aRoot, bool nosound, bool nobgmusic);

    /// \brief Get the localized string for `aStringId`, in the active language of `locale` data member.
    const std::string & translate(StringId aStringId) const;

    filesystem::path pathFor(const filesystem::path &aAsset) const;

    const arte::AnimationSpriteSheet & loadAnimationSpriteSheet(const filesystem::path &aAsset);
    const graphics::sprite::SingleLoad & loadSingleSprite(const filesystem::path &aAsset);
    void loadOggSoundData(const filesystem::path &aAsset, bool streamed);

    Resources resources;
    Locale locale;
    std::unique_ptr<SoundManager> mSoundManager;

    PlayerList mPlayerList;
    
    //Command line switch
    bool nosound;
    bool nobgmusic;
};


//
// Implementations
//
inline Context::Context(const filesystem::path aRoot, bool nosound, bool nobgmusic) :
    resources{{std::move(aRoot)}},
    locale{resources.locator.pathFor("localization/strings.json")},
    nosound{nosound},
    nobgmusic{nobgmusic}
{
    if (!nosound)
    {
        mSoundManager = std::make_unique<SoundManager>();
    }
    else
    {
        mSoundManager = nullptr;
    }
}


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


inline void Context::loadOggSoundData(const filesystem::path &aAsset, bool streamed)
{
    if (!nosound)
    {
        const OggSoundData & data = resources.oggSoundFiles.load(aAsset, resources.locator, streamed);
        mSoundManager->storeDataInLoadedSound(data);
    }
}


} // namespace grapito
} // namespace ad
