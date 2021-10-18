#pragma once

#include "../Components/Body.h"
#include "../Utils/PhysicsStructs.h"
#include <aunteater/EntityManager.h>
#include <aunteater/Component.h>
#include <list>

namespace ad {
namespace grapito
{

struct DistanceJoint : public aunteater::Component<DistanceJoint>
{
    explicit DistanceJoint(
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
        float massA = aBodyA->get<Body>().mass;
        float massB = aBodyB->get<Body>().mass;
        float mass;

        if (massA > 0.f && massB > 0.f)
        {
            mass = massA * massB / (massA + massB);
        }
        else if (massA > 0.f)
        {
            mass = massA;
        }
        else
        {
            mass = massB;
        }

        float omega = 2.f * math::pi<float> * aFrequency;
        mStiffness = mass * omega * omega;
        mDamping = 2.f * mass * aDampingRatio * omega;
    }

    Position2 localAnchorA = Position2::Zero();
    Position2 localAnchorB = Position2::Zero();

    float mStiffness;
    float mDamping;
    float mSlackFactor;

    aunteater::weak_entity bodyA;
    aunteater::weak_entity bodyB;

    std::list<std::unique_ptr<JointConstraint>>::iterator constraintIt;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItA;
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator>::iterator constructedBodyConstraintItB;
};


} // namespace grapito
} // namespace ad
