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
};


} // namespace grapito
} // namespace ad
