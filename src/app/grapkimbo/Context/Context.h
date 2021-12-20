#pragma once


#include "Localization.h"
#include "Resources.h"


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

    Resources resources;
    Locale locale;
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


} // namespace grapito
} // namespace ad
