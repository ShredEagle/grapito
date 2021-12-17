#include "Localization.h"

#include <arte/detail/Json.h>

namespace ad {
namespace grapito {


namespace detail {

    const std::string & TranslationTable::lookup(StringId aStringId, std::size_t aLanguageIndex) const
    {
        return mLocalizedStrings.at(aStringId)[aLanguageIndex];
    }

} // namespace detail

Locale::Locale(filesystem::path aTranslationDatabase)
{
    std::ifstream jsonInput{aTranslationDatabase.string()};
    Json data;
    jsonInput >> data;
    
    for (auto languageName : data.at("languages"))
    {
        mAvailableLanguages.push_back(languageName.get<std::string>());
    }

    for (auto textEntry : data.at("strings"))
    {
        std::vector<std::string> translations;
        translations.reserve(mAvailableLanguages.size());

        for (const std::string & languageName : mAvailableLanguages)
        {
            translations.push_back(textEntry.at(languageName).get<std::string>());
        }

        mTranslations.mLocalizedStrings.emplace(
            StringId{textEntry.at("id").get<std::string>()},
            std::move(translations));
    }
}


void Locale::setLanguage(const std::string & aLanguageName)
{
    if (auto foundIt = std::find(mAvailableLanguages.begin(),
                                 mAvailableLanguages.end(),
                                 aLanguageName);
        foundIt != mAvailableLanguages.end())
    {
        mCurrentLanguage = std::distance(mAvailableLanguages.begin(), foundIt);
    }
    else
    {
        throw std::logic_error{"Language '" + aLanguageName + "' is not available in Locale."};
    }
}


const std::string & Locale::translate(StringId aStringId) const
{
    return mTranslations.lookup(aStringId, mCurrentLanguage);
}


} // namespace grapito
} // namespace ad
