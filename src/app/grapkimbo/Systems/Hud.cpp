#include "Hud.h"

#include "../Configuration.h"
#include "../Timer.h"


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
        switch(screenPosition.origin)
        {
        case ScreenPosition::BottomLeft:
            mTexting.prepareString(text.message, screenPosition.position, strings);
            break;
        case ScreenPosition::Center:
            math::Rectangle<GLfloat> messageBounds = 
                mTexting.getStringBounds(text.message, math::Position<2, GLfloat>::Zero());
            math::Position<2, GLfloat> messageCenter = messageBounds.origin() + messageBounds.center().as<math::Vec>();
            mTexting.prepareString(text.message, 
                                   -messageCenter + screenPosition.position.as<math::Vec>(),
                                   strings);
            break;
        }
    }
    mTexting.updateInstances(strings);
    mTexting.render();
}


} // namespace grapito
} // namespace ad