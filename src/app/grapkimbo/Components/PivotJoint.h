#pragma once

#include "Utils/PhysicsStructs.h"
#include <aunteater/Component.h>
#include <list>

namespace ad {
namespace grapito
{

struct PivotJoint : public aunteater::Component<PivotJoint>
{
    explicit PivotJoint(Position2 aLocalAnchorA, Position2 aLocalAnchorB, aunteater::weak_entity aBodyA, aunteater::weak_entity aBodyB) :
        localAnchorA{std::move(aLocalAnchorA)},
        localAnchorB{std::move(aLocalAnchorB)},
        bodyA{aBodyA},
        bodyB{aBodyB}
    {}

    Position2 localAnchorA = Position2::Zero();
    Position2 localAnchorB = Position2::Zero();

    aunteater::weak_entity bodyA;
    aunteater::weak_entity bodyB;

    size_t constraintIndex;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItA;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItB;
};


} // namespace grapito
} // namespace ad
