#include "RenderHud.h"

#include "../Configuration.h"
#include "../Timer.h"


namespace ad {
namespace grapito {


RenderHud::RenderHud(aunteater::EntityManager & aEntityManager,
                     const filesystem::path & aFontPath,
                     std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mHudTexts{aEntityManager},
    mSizeToTexting{ 
        graphics::Texting{aFontPath, hud::gSmallTextHeight, hud::gViewedHeight, aAppInterface},
        graphics::Texting{aFontPath, hud::gLargeTextHeight, hud::gViewedHeight, aAppInterface},
    }
{}


void RenderHud::update(const GrapitoTimer /*aTimer*/, const GameInputState &)
{
    std::array<graphics::Texting::Mapping, Text::_End> sizeToStrings;

    for(const auto & [screenPosition, text] : mHudTexts)
    {
        graphics::Texting & texting = mSizeToTexting[text.size];
        auto & strings = sizeToStrings[text.size];

        switch(screenPosition.origin)
        {
        case ScreenPosition::BottomLeft:
            texting.prepareString(text.message, screenPosition.position, text.color, strings);
            break;
        case ScreenPosition::Center:
            math::Rectangle<GLfloat> messageBounds = 
                texting.getStringBounds(text.message, math::Position<2, GLfloat>::Zero());
            math::Position<2, GLfloat> messageCenter = messageBounds.origin() + messageBounds.center().as<math::Vec>();
            texting.prepareString(
                text.message, 
                -messageCenter + screenPosition.position.as<math::Vec>(),
                text.color,
                strings);
            break;
        }
    }

    mSizeToTexting[Text::Small].updateInstances(sizeToStrings[Text::Small]);
    mSizeToTexting[Text::Large].updateInstances(sizeToStrings[Text::Large]);
}


void RenderHud::render() const
{
    mSizeToTexting[Text::Small].render();
    mSizeToTexting[Text::Large].render();
}


} // namespace grapito
} // namespace ad
