#pragma once


#include <aunteater/Component.h>


namespace ad {


struct PlayerData : public aunteater::Component<PlayerData>
{
    PlayerData(int aId, Color aColor) :
        id{aId},
        color{aColor}
    {}

    int id;
    Color color;
};


} // namespace ad