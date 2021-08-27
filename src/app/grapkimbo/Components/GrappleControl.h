#pragma once

#include <aunteater/Component.h>


namespace ad {


enum class GrappleMode
{
    AnchorSight,
    Closest,
};


inline GrappleMode toggle(GrappleMode aCurrent)
{
    if (aCurrent == GrappleMode::AnchorSight)
    {
        return GrappleMode::Closest;
    }
    else
    {
        return GrappleMode::AnchorSight;
    }
}


struct GrappleControl : public aunteater::Component<GrappleControl>
{
    GrappleControl(GrappleMode aMode) :
        mode{aMode}
    {}

    GrappleMode mode;
};



} // namespace ad
