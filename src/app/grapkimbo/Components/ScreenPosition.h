#pragma once

#include "../commons.h"

#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct ScreenPosition : public aunteater::Component<ScreenPosition>
{
    ScreenPosition(Position2 aPosition) :
        position{aPosition}
    {}

    Position2 position;
};


} // namespace grapito
} // namespace ad
