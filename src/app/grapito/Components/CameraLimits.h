#pragma once


#include <aunteater/Component.h>


namespace ad {
namespace grapito {


/// \brief Control the limits of what the camera must try to show, relative to the entity position.
struct CameraLimits : public aunteater::Component<CameraLimits>
{
    CameraLimits(float aAbove, float aBelow) :
        above{aAbove},
        below{aBelow}
    {}

    float above{0.f};
    float below{0.f};
};


} // namespace grapito
} // namespace ad
