#pragma once

#include "aunteater/EntityManager.h"
#include <aunteater/Component.h>
#include <vector>

namespace ad {
namespace grapito
{

struct RopeCreator : public aunteater::Component<RopeCreator>
{
    explicit RopeCreator(aunteater::weak_entity aEntity) :
        mTargetEntity{aEntity}
    {}

    std::vector<aunteater::weak_entity> mRopeSegments;
    aunteater::weak_entity mTargetEntity;
};


} // namespace grapito
} // namespace ad
