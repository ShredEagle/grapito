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

    std::list<std::unique_ptr<JointConstraint>>::iterator constraintIt;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItA;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItB;
};


} // namespace grapito
} // namespace ad
