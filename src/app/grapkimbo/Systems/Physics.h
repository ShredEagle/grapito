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

static inline void addPair(ConstructedBody & bodyA, ConstructedBody & bodyB, std::list<CollisionPair> & pairs)
{
    for (auto contact : bodyA.contactList)
    {
        if (
            (bodyA.entity == contact->bodyA.entity && bodyB.entity == contact->bodyB.entity) ||
            (bodyA.entity == contact->bodyB.entity && bodyB.entity == contact->bodyA.entity)
        )
        {
            contact->cold = false;
            //contact already exists so we get out
            return;
        }
    }

    CollisionPair contactPair = {
        false,
        bodyA,
        bodyB,
    };

    pairs.push_back(std::move(contactPair));

    pairs.back().iteratorA = bodyA.contactList.insert(bodyA.contactList.end(), &(pairs.back()));
    pairs.back().iteratorB = bodyB.contactList.insert(bodyB.contactList.end(), &(pairs.back()));
}

static inline const double distanceToLine(Position2 point, Position2 origin, Position2 end, Vec2 normal)
{
    return (((end.x() - origin.x()) * (origin.y() - point.y())) - ((origin.x() - point.x()) * (end.y() - origin.y()))) / (origin - end).getNorm();
}

static inline ContactManifold QueryFacePenetration(
    const ConstructedBody & bodyA,
    const ConstructedBody & bodyB
)
{
    ContactManifold resultManifold{.localPoint = Vec2::Zero()};

    for (int i = 0; i < bodyA.box->shape.mFaceCount; ++i)
    {
        const Shape::Edge edgeA = bodyA.box->shape.getEdge(i);
        const Position2 support = bodyB.box->getSupport(-edgeA.normal);
        const double distance = distanceToLine(support, edgeA.origin, edgeA.end, edgeA.normal);

        Color color = distance > 0 ? Color{0, 200, 0} : Color{200, 0, 0};


        if (distance > resultManifold.separation)
        {
            resultManifold.separation = distance;
            resultManifold.referenceEdgeIndex = i;
            resultManifold.normal = -edgeA.normal;
            resultManifold.localPoint = edgeA.origin - bodyA.bodyPos->c;
        }
    }

    return resultManifold;
};

static inline void applyImpulse(
        Vec2 impVecA,
        double angularImpulseA,
        Vec2 impVecB,
        double angularImpulseB,
        PivotJointConstraint & constraint,
        double delta
        )
{
    constraint.bodyPosA->p += impVecA * delta;
    constraint.bodyPosA->a += math::Radian<double>{angularImpulseA * delta};
    constraint.velocityA->v += impVecA;
    constraint.velocityA->w += angularImpulseA;

    constraint.bodyPosB->p += impVecB * delta;
    constraint.bodyPosB->a += math::Radian<double>{angularImpulseB * delta};
    constraint.velocityB->v += impVecB;
    constraint.velocityB->w += angularImpulseB;
}

static inline void applyImpulse(
        Vec2 impVecA,
        double angularImpulseA,
        Vec2 impVecB,
        double angularImpulseB,
        VelocityConstraint & constraint,
        double delta
        )
{
    constraint.bodyPosA->p += impVecA * delta;
    constraint.bodyPosA->a += math::Radian<double>{angularImpulseA * delta};
    constraint.velocityA->v += impVecA;
    constraint.velocityA->w += angularImpulseA;

    constraint.bodyPosB->p += impVecB * delta;
    constraint.bodyPosB->a += math::Radian<double>{angularImpulseB * delta};
    constraint.velocityB->v += impVecB;
    constraint.velocityB->w += angularImpulseB;
}


static constexpr int maxNormalConstraintIteration = 8;
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

class Physics : public aunteater::System<GameInputState>
{

public:
    Physics(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;


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
