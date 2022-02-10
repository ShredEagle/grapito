#pragma once


#include "../Configuration.h"

#include <aunteater/Component.h>

#include <graphics/Sprite.h>
#include <graphics/SpriteLoading.h>


namespace ad {
namespace grapito {


struct VisualSprite : public aunteater::Component<VisualSprite>
{
    enum Alignment
    {
        AlignLeft,
        AlignRight,
    };

    VisualSprite(AtlasIndex aAtlas = 0) :
        atlas{aAtlas},
        sprite{ {0, 0}, {0, 0} }
    {}


    VisualSprite(AtlasIndex aAtlas, graphics::LoadedSprite aSprite) :
        atlas{aAtlas},
        sprite{aSprite}
    {}

    Size2 getWorldSize() const
    {
        return render::gSpritePixelWorldSize * static_cast<Size2>(sprite.dimension());
    }

    AtlasIndex atlas;
    graphics::LoadedSprite sprite;
    math::Vec<2, int> mirroring{1, 1};
    Alignment alignment{AlignRight};
};


} // namespace grapito
} // namespace ad
