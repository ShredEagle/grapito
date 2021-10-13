#pragma once

#include "Input.h"

#include <Components/AccelAndSpeed.h>
#include <Components/PivotJoint.h>
#include <Components/Position.h>
#include <Components/Body.h>
#include "Components/VisualRectangle.h"

#include "Utils/CollisionBox.h"
#include "Utils/PhysicsStructs.h"
#include "aunteater/EntityManager.h"
#include "aunteater/Family.h"
#include "commons.h"
#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Position, Body, AccelAndSpeed> PhysicalBody;
typedef aunteater::Archetype<PivotJoint> Pivotable;
static constexpr int maxVelocityConstraintIteration = 16;
static constexpr int maxPositionConstraintIteration = 16;
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

class PivotObserver : public aunteater::FamilyObserver
{
    public:

    PivotObserver(Physics * aPhysicsSystem);

    void addedEntity(aunteater::LiveEntity & aEntity) override;

    void removedEntity(aunteater::LiveEntity & aEntity) override;

    Physics * mPhysicsSystem;
};

class Physics : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Physics(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState & aInputState) override;


private:
    friend BodyObserver;
    friend PivotObserver;

    BodyObserver bodyObserver;
    PivotObserver pivotObserver;

    std::list<ConstructedBody> constructedBodies;
    std::list<CollisionPair> collidingBodies;

    std::vector<Velocity> velocities;
    std::vector<BodyPosition> positions;
    std::vector<CollisionBox> collisionBoxes;

    std::vector<VelocityConstraint> velocityConstraints;
    std::vector<PlayerEnvironmentConstraint> playerConstraints;

    //This is a list for the moment because it is easier to remove stuff
    //from it if it is
    //And there will probably not be that much joint constraint in it
    std::list<PivotJointConstraint> pivotJointConstraints;
    //Think of putting in place a pool_allocator from foonathan/memory of from boost
};


} // namespace grapito
} // namespace ad
