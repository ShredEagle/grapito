#include "Hud.h"

#include "../Configuration.h"


namespace ad {
namespace grapito {


Hud::Hud(aunteater::EntityManager & aEntityManager,
         const filesystem::path & aFontPath,
         std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mHudTexts{aEntityManager},
    mTexting{aFontPath, hud::gTextHeight, hud::gViewedHeight, std::move(aAppInterface)}
{}


void Hud::update(const GrapitoTimer aTimer, const GameInputState &)
{
    graphics::Texting::Mapping strings;

    for(const auto & [screenPosition, text] : mHudTexts)
    {
        mTexting.prepareString(text.message, screenPosition.position, strings);
    }
    mTexting.updateInstances(strings);
    mTexting.render();
}


} // namespace grapito
} // namespace ad