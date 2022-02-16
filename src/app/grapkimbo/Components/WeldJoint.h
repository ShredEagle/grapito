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

    size_t constraintIndex;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItA;
    std::list<aunteater::weak_entity>::iterator constructedBodyEntityItB;
};


} // namespace grapito
} // namespace ad
