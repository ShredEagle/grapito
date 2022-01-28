#pragma once

#include "Input.h"

#include "../commons.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/Body.h"
#include "../Components/PivotJoint.h"
#include "../Components/Position.h"
#include "../Components/VisualRectangle.h"
#include "../Components/WeldJoint.h"
#include "../Components/DistanceJoint.h"

#include "../Utils/CollisionBox.h"
#include "../Utils/PhysicsStructs.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Family.h>
#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Position, Body, AccelAndSpeed> PhysicalBody;
typedef aunteater::Archetype<PivotJoint> Pivotable;
typedef aunteater::Archetype<WeldJoint> Weldable;
typedef aunteater::Archetype<DistanceJoint> Distanceable;

static std::array<
    std::array<
        std::function<ContactManifold(
                const ConstructedBody & bodyA,
                const ConstructedBody & bodyB
                )>,
        ShapeType_count
    >,
    ShapeType_count
> queryFunctions;

class Physics;

class BodyObserver : public aunteater::FamilyObserver
{
    public:

    BodyObserver(Physics * aPhysicsSystem);

    void addedEntity(aunteater::LiveEntity & aEntity) override;

    void removedEntity(aunteater::LiveEntity & aEntity) override;

    Physics * mPhysicsSystem;
};

template<class T_joint, class T_joint_constraint>
class JointObserver : public aunteater::FamilyObserver
{
    public:
    JointObserver(Physics * aPhysicsSystem);

    void addedEntity(aunteater::LiveEntity & aEntity) override;

    void removedEntity(aunteater::LiveEntity & aEntity) override;

    Physics * mPhysicsSystem;
};

class Physics : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Physics(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState & aInputState) override;

    //This is here to simplify the implementation of JointObserver
    std::list<std::unique_ptr<JointConstraint>> jointConstraints;
    std::list<ConstructedBody> constructedBodies;

private:
    friend BodyObserver;

    BodyObserver bodyObserver;
    JointObserver<PivotJoint, PivotJointConstraint> pivotObserver;
    JointObserver<WeldJoint, WeldJointConstraint> weldObserver;
    JointObserver<DistanceJoint, DistanceJointConstraint> distanceObserver;

    std::list<CollisionPair> collidingBodies;

    std::vector<Velocity> velocities;
    std::vector<BodyPosition> positions;
    std::vector<CollisionBox> collisionBoxes;

    std::vector<VelocityConstraint> velocityConstraints;
    std::vector<PlayerEnvironmentConstraint> playerConstraints;

    aunteater::EntityManager & mEntityManager;

    //Think of putting in place a pool_allocator from foonathan/memory of from boost
};


} // namespace grapito
} // namespace ad
