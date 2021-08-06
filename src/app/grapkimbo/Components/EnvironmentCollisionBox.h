#pragma once

#include <aunteater/Component.h>

#include <math/Rectangle.h>

namespace ad {

struct EnvironmentCollisionBox : public aunteater::Component<EnvironmentCollisionBox>
{
    explicit EnvironmentCollisionBox(math::Rectangle<int> aBox = math::Rectangle<int>{{0,0}, {1,1}}):
        box{std::move(aBox)}
    {}

    math::Rectangle<int> box;
    std::vector<aunteater::weak_entity> collidingWith;
};


} // namespace ad
