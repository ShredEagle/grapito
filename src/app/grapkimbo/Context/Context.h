#pragma once


#include "Localization.h"

#include <resource/ResourceManager.h>


namespace ad {
namespace grapito {


/// \brief Intended to be passed around in different places of the code.
/// It centralizes different objects that would otherwise easily become globals.
struct Context
{
    Context(const filesystem::path aRoot);

    /// \brief Get the localized string for `aStringId`, in the active language of `locale` data member.
    const std::string & translate(StringId aStringId);

    resource::ResourceManager resources;
    Locale locale;
};


//
// Implementations
//
inline Context::Context(const filesystem::path aRoot) :
    resources{std::move(aRoot)},
    locale{resources.pathFor("localization/strings.json")}
{}


inline const std::string & Context::translate(StringId aStringId)
{
    return locale.translate(aStringId);
}


} // namespace grapito
} // namespace ad
