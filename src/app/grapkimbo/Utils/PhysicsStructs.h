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
struct CollisionPair;

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
    friend std::ostream &operator<<(std::ostream & os, const ContactFeature & cm);
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

struct Velocity
{
    explicit Velocity(Vec2 aV, double aW) :
        v{aV},
        w{aW}
    {}

    Vec2 v;
    double w;
};

struct BodyPosition
{
    explicit BodyPosition(Position2 aP, Position2 aC, math::Radian<double> aA) :
        p{aP},
        c{aC},
        a{aA}
    {}

    Position2 p;
    Position2 c;
    math::Radian<double> a;
};

class ConstructedBody
{
    public:
    ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity);

    void debugRender();

    void synchronize(std::vector<Velocity> & velocities, std::vector<BodyPosition> & bodyPoses, std::vector<CollisionBox> & collisionBoxes, int index);

    void updateEntity();

    double mass;
    double invMass;
    double moi;
    double invMoi;
    double friction;

    //non owning pointer to Physics system vector
    Velocity * velocity;
    BodyPosition * bodyPos;
    CollisionBox * box;

    double radius;

    //non owning pointer to Physics system vector
    std::list<CollisionPair *>  contactList;

    BodyType bodyType;
    ShapeType shapeType;
    CollisionType collisionType;

    Body & bodyRef;
    Position & posRef;
    AccelAndSpeed & aasRef;
    aunteater::weak_entity entity;

    friend std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb);
};

struct CollisionPair;

struct CollisionPair
{
    bool cold;

    ConstructedBody & bodyA;
    ConstructedBody & bodyB;
    std::list<CollisionPair *>::iterator iteratorA;
    std::list<CollisionPair *>::iterator iteratorB;

    ContactManifold manifold;

    void debugRender();
};

struct VelocityConstraint
{
    //non owning pointer to Physics system vector
    Velocity * velocityA;
    BodyPosition * bodyPosA;
    Vec2 rA;
    double crossA;
    double crossATangent;
    double invMassA;
    double invMoiA;
    double tangentSpeedA;

    //non owning pointer to Physics system vector
    Velocity * velocityB;
    BodyPosition * bodyPosB;
    Vec2 rB;
    double crossB;
    double crossBTangent;
    double invMassB;
    double invMoiB;
    double tangentSpeedB;

    double friction;
    double restitution;
    Vec2 normal; 
    Vec2 tangent;

    ContactFeature & cf;
    ConstructedBody * cbA;
    ConstructedBody * cbB;

    void debugRender();

    friend std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc);
};

struct PlayerEnvironmentConstraint
{
    Vec2 normal; 
    double separation;

    ConstructedBody * cPlayer;
};
}
}
