#pragma once

#include "../Components/Body.h"
#include "../Utils/PhysicsStructs.h"
#include <aunteater/EntityManager.h>
#include <aunteater/Component.h>
#include <list>

namespace ad {
namespace grapito
{

struct WeldJoint : public aunteater::Component<WeldJoint>
{
    explicit WeldJoint(
            Position2 aLocalAnchorA,
            Position2 aLocalAnchorB,
            float aFrequency,
            float aDampingRatio,
            aunteater::weak_entity aBodyA,
            aunteater::weak_entity aBodyB
            ) :
        localAnchorA{std::move(aLocalAnchorA)},
        localAnchorB{std::move(aLocalAnchorB)},
        bodyA{aBodyA},
        bodyB{aBodyB}
    {
        computeStiffnessDamping(
                mStiffness,
                mDamping,
                aFrequency,
                aDampingRatio,
                aBodyA->get<Body>().mass,
                aBodyB->get<Body>().mass
                );
    }

    Position2 localAnchorA = Position2::Zero();
    Position2 localAnchorB = Position2::Zero();

    float mStiffness;
    float mDamping;

    aunteater::weak_entity bodyA;
    aunteater::weak_entity bodyB;

    std::list<std::unique_ptr<JointConstraint>>::iterator constraintIt;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItA;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItB;
};


} // namespace grapito
} // namespace ad
