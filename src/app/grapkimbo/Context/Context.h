#pragma once


#include "Localization.h"

#include <resource/ResourceLocator.h>


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

    resource::ResourceLocator resourceLocator;
    Locale locale;
};


//
// Implementations
//
inline Context::Context(const filesystem::path aRoot) :
    resourceLocator{std::move(aRoot)},
    locale{resourceLocator.pathFor("localization/strings.json")}
{}


inline const std::string & Context::translate(StringId aStringId) const
{
    return locale.translate(aStringId);
}


inline filesystem::path Context::pathFor(const filesystem::path &aAsset) const
{
    return resourceLocator.pathFor(aAsset);
}


} // namespace grapito
} // namespace ad
