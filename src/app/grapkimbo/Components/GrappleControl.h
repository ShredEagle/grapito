#pragma once

#include <aunteater/Component.h>


namespace ad {


enum class GrappleMode
{
    AnchorSight,
    Closest,
};

struct GrappleControl : public aunteater::Component<GrappleControl>
{
    GrappleControl(GrappleMode aMode) :
        mode{aMode}
    {}

    GrappleMode mode;
};



} // namespace ad
