#pragma once


#include "commons.h"

#include <platform/Filesystem.h>

#include <unordered_map>
#include <string>


namespace ad {
namespace grapito {


namespace detail {

    class TranslationTable
    {
    public:
        const std::string & lookup(StringId aStringId, std::size_t aLanguageIndex) const;

        // Associate a StringId to a vector of corresponding std::strings.
        // The index of the string matches its languages, in the order it appears in Json "languages".
        std::unordered_map<StringId, std::vector<std::string>> mLocalizedStrings;
    };

} // namespace detail


/// \brief Provide string translations for the currently selected language.
class Locale
{
public:
    Locale(filesystem::path aTranslationDatabase);

    /// \brief Indicate whether the provided language is available in the translation database.
    bool isSupported(const std::string & aLanguageName);

    /// \brief Set the active language. Default is "zero" (i.e. first language in the Json).
    void setLanguage(const std::string & aLanguageName);

    /// \brief Get the localized string for `aStringId`, in the active language.
    const std::string & translate(StringId aStringId) const;

private:
    std::vector<std::string> mAvailableLanguages;
    detail::TranslationTable mTranslations;
    std::size_t mCurrentLanguage{0};
};


} // namespace grapito
} // namespace ad
