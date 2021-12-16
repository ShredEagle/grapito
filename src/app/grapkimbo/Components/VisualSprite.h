#pragma once


#include <aunteater/Component.h>

#include <graphics/Sprite.h>


namespace ad {
namespace grapito {


struct VisualSprite : public aunteater::Component<VisualSprite>
{
    VisualSprite() :
        sprite{ {0, 0}, {0, 0} }
    {}

    graphics::LoadedSprite sprite;
    math::Vec<2, int> mirroring{1, 1};
};


} // namespace grapito
} // namespace ad
