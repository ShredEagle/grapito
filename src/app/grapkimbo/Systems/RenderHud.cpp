#include "RenderHud.h"

#include "../Configuration.h"
#include "../Timer.h"


namespace ad {
namespace grapito {


RenderHud::RenderHud(aunteater::EntityManager & aEntityManager,
                     const filesystem::path & aFontPath,
                     std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mHudTexts{aEntityManager},
    mTexting{aFontPath, hud::gTextHeight, hud::gViewedHeight, std::move(aAppInterface)}
{}


void RenderHud::update(const GrapitoTimer aTimer, const GameInputState &)
{
    graphics::Texting::Mapping strings;

    for(const auto & [screenPosition, text] : mHudTexts)
    {
        switch(screenPosition.origin)
        {
        case ScreenPosition::BottomLeft:
            mTexting.prepareString(text.message, screenPosition.position, text.color, strings);
            break;
        case ScreenPosition::Center:
            math::Rectangle<GLfloat> messageBounds = 
                mTexting.getStringBounds(text.message, math::Position<2, GLfloat>::Zero());
            math::Position<2, GLfloat> messageCenter = messageBounds.origin() + messageBounds.center().as<math::Vec>();
            mTexting.prepareString(text.message, 
                                   -messageCenter + screenPosition.position.as<math::Vec>(),
                                   text.color,
                                   strings);
            break;
        }
    }
    mTexting.updateInstances(strings);
}


void RenderHud::render() const
{
    mTexting.render();
}


} // namespace grapito
} // namespace ad