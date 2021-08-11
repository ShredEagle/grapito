#pragma once


#include <aunteater/Component.h>

#include <math/Rectangle.h>

#include <vector>


namespace ad {

struct EnvironmentCollisionBox : public aunteater::Component<EnvironmentCollisionBox>
{
    explicit EnvironmentCollisionBox(math::Rectangle<double> aBox = math::Rectangle<double>{{0., 0.}, {1. , 1.}}):
        box{std::move(aBox)}
    {}

    math::Rectangle<double> box;
    std::vector<aunteater::weak_entity> collidingWith;
};


} // namespace ad
