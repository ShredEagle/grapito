#pragma once


#include "../Input.h"

#include "../Components/ScreenPosition.h"
#include "../Components/Text.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <graphics/Texting.h>


namespace ad {
namespace grapito {


using HudText = aunteater::Archetype<ScreenPosition, Text>;


class RenderHud : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    RenderHud(aunteater::EntityManager & aEntityManager,
              const filesystem::path & aFontPath,
              std::shared_ptr<graphics::AppInterface> aAppInterface);

    void update(const GrapitoTimer, const GameInputState &) override;

    void render() const;

private:
    const aunteater::FamilyHelp<HudText> mHudTexts;

    std::array<graphics::Texting, Text::_End> mSizeToTexting;
};


} // namespace grapito
} // namespace ad
