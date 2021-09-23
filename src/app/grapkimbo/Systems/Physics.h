#pragma once

#include "Input.h"

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

#include "Utils/CollisionBox.h"
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

struct ContactFeature
{
    enum Type : uint8_t
    {
        vertex,
        face
    };
    
    Type typeReference;
    Type typeIncident;
    uint8_t indexReference; 
    uint8_t indexIncident; 

    Position2 contactPoint = Position2::Zero();
    float normalImpulse;
    float tangentImpulse;
};

struct ContactManifold
{
    enum ReferenceFace
    {
        FACEA,
        FACEB,
    };

    ReferenceFace face;
    uint_fast8_t referenceEdgeIndex;
    uint_fast8_t incidentEdgeIndex;

    Vec2 normal = Vec2::Zero();
    double separation = -std::numeric_limits<double>::max();

    std::vector<ContactFeature> contacts;
};

struct Line
{
    Position2 origin;
    Vec2 direction;
};

struct LightBody
{

};

class ConstructedBody
{
    public:
    ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity) :
        mass{aBody.mass},
        invMass{aBody.invMass},
        moi{aBody.moi},
        invMoi{aBody.invMoi},
        friction{aBody.friction},
        speed{aAas.speed},
        w{aAas.w},
        massCenter{aBody.massCenter},
        position{aPos.position},
        theta{aBody.theta},
        radius{aBody.radius},
        box{createTransformedCollisionBox(aBody, aPos)},
        bodyType{aBody.bodyType},
        shapeType{aBody.shapeType},
        bodyRef{aBody},
        posRef{aPos},
        aasRef{aAas},
        entity{aEntity}
    {
    }

    static std::vector<Position2> createTransformedCollisionBox(Body & aBody, Position & pos)
    {
        std::vector<Position2> transformedVertices;

        for (auto vertex : aBody.shape.vertices)
        {
            auto transformedPos = transformPosition(vertex, aBody.theta, pos.position);
            transformedVertices.emplace_back(transformedPos);
        }

        return transformedVertices;
    }

    void synchronize()
    {
        box = CollisionBox{createTransformedCollisionBox(bodyRef, posRef)};
    }

    void updateEntity()
    {
        bodyRef.theta = theta;
        aasRef.speed = speed;
        aasRef.w = w;
        posRef.position = position;
    }

    double mass;
    double invMass;
    double moi;
    double invMoi;
    double friction;
    Vec2 speed;
    double w;
    Position2 massCenter;

    Position2 position;
    math::Radian<double> theta;

    double radius;

    CollisionBox box;

    BodyType bodyType;
    ShapeType shapeType;

    std::vector<std::reference_wrapper<ContactManifold>> persistedManifold;

    Body & bodyRef;
    Position & posRef;
    AccelAndSpeed & aasRef;
    aunteater::weak_entity entity;
};

struct VelocityConstraint
{
    Vec2 vA;
    double wA;
    double invMassA;
    double invMoiA;
    double tangentSpeedA;

    Vec2 vB;
    double wB;
    double invMassB;
    double invMoiB;
    double tangentSpeedB;

    double friction;
    double restitution;
    Vec2 normal; 

    ContactFeature & cf;
    ConstructedBody * cbA;
    ConstructedBody * cbB;
};

struct CollisionPair
{
    ConstructedBody & bodyA;
    ConstructedBody & bodyB;

    ContactManifold manifold;
};

static inline void addPair(ConstructedBody & bodyA, ConstructedBody & bodyB, std::list<CollisionPair> & pairs)
{
    for (auto contact : bodyA.bodyRef.contactList)
    {
        if (
            (bodyA.entity == contact.bodyA.entity && bodyB.entity == contact.bodyB.entity) ||
            (bodyA.entity == contact.bodyB.entity && bodyB.entity == contact.bodyA.entity)
        )
        {
            //contact already exists so we get out
            return;
        }
    }

    CollisionPair contactPair = {bodyA, bodyB};

    bodyA.bodyRef.contactList.emplace_back(contactPair);
    bodyB.bodyRef.contactList.emplace_back(contactPair);

    pairs.push_back({bodyA, bodyB});
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
    ContactManifold resultManifold;

    for (int i = 0; i < bodyA.box.shape.mFaceCount; ++i)
    {
        const Shape::Edge edgeA = bodyA.box.shape.getEdge(i);
        const Position2 support = bodyB.box.getSupport(-edgeA.normal);
        const double distance = distanceToLine(support, edgeA.origin, edgeA.end, edgeA.normal);

        Color color = distance > 0 ? Color{0, 200, 0} : Color{200, 0, 0};


        if (distance > resultManifold.separation)
        {
            resultManifold.separation = distance;
            resultManifold.referenceEdgeIndex = i;
            resultManifold.normal = -edgeA.normal;
        }
    }

    return resultManifold;
};

static inline void applyImpulse(Vec2 impVec, double angularImpulse, ConstructedBody * body, double delta)
{
    body->position += impVec * delta;
    body->theta += math::Radian<double>{angularImpulse * delta};
    body->speed += impVec;

    body->w += angularImpulse;
}

class Physics : public aunteater::System<GameInputState>
{

public:
    Physics(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;


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

private:
    class PhysicalBodyObserver : public aunteater::FamilyObserver
    {
        void addedEntity(aunteater::LiveEntity & aEntity)
        {
            aEntity.get<Body>().constructedBodyIt =
                constructedBodies.insert(
                        constructedBodies.end(),
                        ConstructedBody{aEntity.get<Body>(), aEntity.get<Position>(), aEntity.get<AccelAndSpeed>(), &aEntity}
                        );
        }

        void removedEntity(aunteater::LiveEntity & aEntity)
        {
            constructedBodies.erase(aEntity.get<Body>().constructedBodyIt);
        }
    };
    
    PhysicalBodyObserver observer;

    //Think of putting in place a pool_allocator from foonathan/memory of from boost
    static std::list<ConstructedBody> constructedBodies;
    static std::list<CollisionPair> collidingBodies;
    static std::vector<VelocityConstraint> velocityConstraints;

    static constexpr int maxNormalConstraintIteration = 5;
    static constexpr int maxTangentConstraintIteration = 5;
};


} // namespace grapito
} // namespace ad
