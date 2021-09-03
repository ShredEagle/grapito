#pragma once

#include <aunteater/Component.h>

namespace ad {
namespace grapito
{


struct PlayerData : public aunteater::Component<PlayerData>
{
    PlayerData(int aId, Color aColor) :
        id{aId},
        color{aColor}
    {}

    int id;
    Color color;
};


} // namespace grapito
} // namespace ad
