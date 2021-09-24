#pragma once
#include "Components/AccelAndSpeed.h"
#include "Components/Position.h"
#include "Utils/CollisionBox.h"
#include "commons.h"
#include "math/Angle.h"
#include <bits/stdint-uintn.h>
namespace ad {
namespace grapito {

struct Body;

enum ShapeType
{
    ShapeType_Hull,
    ShapeType_count,
};

enum CollisionType
{
    CollisionType_Player,
    CollisionType_Moving_Env,
    CollisionType_Floor,
    CollisionType_Static_Env,
    CollisionType_count,
};

enum BodyType
{
    BodyType_Static,
    BodyType_Dynamic,
};

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
    enum ReferenceFace : int
    {
        FACEA,
        FACEB,
    };

    ReferenceFace face;
    int referenceEdgeIndex;
    int incidentEdgeIndex;

    Vec2 normal = Vec2::Zero();
    double separation = -std::numeric_limits<double>::max();

    std::vector<ContactFeature> contacts;
    friend std::ostream &operator<<(std::ostream & os, const ContactManifold & cm);
};

struct Line
{
    Position2 origin;
    Vec2 direction;
};

class ConstructedBody
{
    public:
    ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity);

    static std::vector<Position2> createTransformedCollisionBox(Body & aBody, Position & pos);

    void debugRender();

    void synchronize();

    void updateEntity();

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

    Body & bodyRef;
    Position & posRef;
    AccelAndSpeed & aasRef;
    aunteater::weak_entity entity;

    friend std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb);
};

struct CollisionPair
{
    ConstructedBody & bodyA;
    ConstructedBody & bodyB;
    bool cold;

    ContactManifold manifold;

    void debugRender();
};

struct VelocityConstraint
{
    Vec2 vA;
    Position2 cA;
    Position2 pA;
    math::Radian<double> aA;
    double wA;
    double invMassA;
    double invMoiA;
    double tangentSpeedA;

    Vec2 vB;
    Position2 cB;
    Position2 pB;
    math::Radian<double> aB;
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

    void debugRender();

    friend std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc);
};
}
}
