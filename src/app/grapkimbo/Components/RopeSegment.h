#pragma once


#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct RopeSegment : public aunteater::Component<RopeSegment>
{
    RopeSegment(int aPlayerId) :
        playerId{aPlayerId}
    {}

    int playerId;
};


} // namespace grapito
} // namespace ad
