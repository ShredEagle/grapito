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
        DistanceJoint(aLocalAnchorA, aLocalAnchorB, aFrequency, aDampingRatio, 0.f, 0.f, aBodyA, aBodyB)
    {
    }
    
    explicit DistanceJoint(
            Position2 aLocalAnchorA,
            Position2 aLocalAnchorB,
            float aFrequency,
            float aDampingRatio,
            float aMinSlackFactor,
            float aMaxSlackFactor,
            float aLength,
            aunteater::weak_entity aBodyA,
            aunteater::weak_entity aBodyB
            ) :
        localAnchorA{std::move(aLocalAnchorA)},
        localAnchorB{std::move(aLocalAnchorB)},
        mMinSlackFactor{aMinSlackFactor},
        mMaxSlackFactor{aMaxSlackFactor},
        mLength{aLength},
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

    explicit DistanceJoint(
            Position2 aLocalAnchorA,
            Position2 aLocalAnchorB,
            float aFrequency,
            float aDampingRatio,
            float aMinSlackFactor,
            float aMaxSlackFactor,
            aunteater::weak_entity aBodyA,
            aunteater::weak_entity aBodyB
            ) :
        DistanceJoint(aLocalAnchorA, aLocalAnchorB, aFrequency, aDampingRatio, aMinSlackFactor, aMaxSlackFactor, -1.f, aBodyA, aBodyB)
    {
    }

    Position2 localAnchorA = Position2::Zero();
    Position2 localAnchorB = Position2::Zero();

    float mStiffness;
    float mDamping;
    float mMinSlackFactor = 0.f;
    float mMaxSlackFactor = 0.f;
    float mLength = 0.f;

    aunteater::weak_entity bodyA;
    aunteater::weak_entity bodyB;

    size_t constraintIndex;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItA;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItB;
};


} // namespace grapito
} // namespace ad
