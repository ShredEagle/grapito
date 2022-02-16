#pragma once

#include "../commons.h"

#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct ScreenPosition : public aunteater::Component<ScreenPosition>
{
    enum Origin
    {
        BottomLeft,
        Center,
    };

    ScreenPosition(Position2 aPosition, Origin aEntityOrigin) :
        position{aPosition},
        origin{aEntityOrigin}
    {}

    Position2 position;
    Origin origin; // The origin of the positioned entity
};


} // namespace grapito
} // namespace ad
