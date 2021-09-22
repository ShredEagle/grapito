#pragma once

#include "Input.h"

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

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
    
    Type typeA;
    Type typeB;
    uint8_t indexA; 
    uint8_t indexB; 

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

    Vec2 normal;
    double separation;

    std::vector<ContactFeature> contacts;
};


class ConstructedBody
{
    public:
    ConstructedBody(Body & aBody, Position & aPos) :
        mass{aBody.mass},
        invMass{aBody.invMass},
        moi{aBody.moi},
        invMoi{aBody.invMoi},
        friction{aBody.friction},
        massCenter{aBody.massCenter},
        radius{aBody.radius},
        bodyType{aBody.bodyType},
        shapeType{aBody.shapeType},
        bodyRef{aBody},
        posRef{aPos},
        box{createTransformedCollisionBox(aBody, aPos)}
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

    double mass;
    double invMass;
    double moi;
    double invMoi;
    double friction;
    Position2 massCenter;

    double radius;

    CollisionBox box;

    BodyType bodyType;
    ShapeType shapeType;
    std::vector<ContactManifold> manifolds;
    Body & bodyRef;
    Position & posRef;
};

/*
struct ContactQuery
{
    ContactQuery(
        const aunteater::weak_entity aEntity
    ) :
        entity{aEntity}
    {};


#ifdef KIMBO_DEBUG
    void debugRender(Color vecColor)
    {
        for (auto contact : contacts)
        {
            debugDrawer->drawPoint(
                {
                    contact.point,
                    .05,
                    Color{0, 0, 255}
                }
            );
            debugDrawer->drawPoint(
                {
                    point,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    origin,
                    end,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    contact.point,
                    contact.point - normal * distance,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    (origin + static_cast<Vec2>(end)) / 2,
                    (origin + static_cast<Vec2>(end)) / 2 + normal * 20,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawPoint({
                end,
                2.,
                Color{0,255,0}
            });
            debugDrawer->drawPoint({
                origin,
                2.,
            });
        }
    }
#endif

#ifdef KIMBO_DEBUG
    Position2 origin = Position2::Zero();
    Position2 end = Position2::Zero();
    Position2 point = Position2::Zero();
#endif

    aunteater::weak_entity entity;
    double distance = 0.;
    Vec2 normal = Vec2::Zero();

    int index = -1;
    int incEdgeIndex = -1;
    ReferenceFace face;

    std::vector<Contact> contacts;
};
*/

class Physics : public aunteater::System<GameInputState>
{

public:
    Physics(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    class PhysicalBodyObserver : public aunteater::FamilyObserver
    {
        void addedEntity(aunteater::LiveEntity & aEntity)
        {
            aEntity.get<Body>().constructedBodyIt =
                constructedBodies.insert(
                        constructedBodies.end(),
                        ConstructedBody{aEntity.get<Body>(), aEntity.get<Position>()}
                        );
        }

        void removedEntity(aunteater::LiveEntity & aEntity)
        {
            constructedBodies.erase(aEntity.get<Body>().constructedBodyIt);
        }
    };

    static std::list<ConstructedBody> constructedBodies;
    static std::vector<std::array<ConstructedBody&, 2>> collidingBodies;
    static std::vector<ContactManifold> contactManifolds;
};

} // namespace grapito
} // namespace ad
