#pragma once
#include "Components/AccelAndSpeed.h"
#include "Components/Position.h"
#include "Utils/CollisionBox.h"
#include "commons.h"
#include "math/Angle.h"
#include "math/Matrix.h"
namespace ad {
namespace grapito {

struct Body;
struct CollisionPair;
struct PivotJointConstraint;

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
    BodyType_Kinematic,
};

//This is the structure that stores the identification of a
//contact point between two body
//It also stores the impulse for that contact for persistence
//between frames
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

//A contact manifold the data for a collision between two
//body, which body is the reference body and the
//indexes of the face colliding
//It contains the list of the contact point
struct ContactManifold
{
    enum ReferenceFace : int
    {
        FACEA,
        FACEB,
    };

    ReferenceFace face;
    int referenceEdgeIndex = 0;
    int incidentEdgeIndex = 0;
    Vec2 localPoint = Vec2::Zero();

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

//The structures that hold the velocity of a body
//This is stored in a vector in the physics system
//Its purpose is to make the velocities of the body
//contiguous in memory
struct Velocity
{
    explicit Velocity(Vec2 aV, double aW) :
        v{aV},
        w{aW}
    {}

    Vec2 v;
    double w;
};

//Same as velocity but for the position of a body
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

//A ConstructedBody is a proxy representation of a Body component
//It's created when a Body component is added to an entity
//This is the central structure for physics resolution for the physics system
class ConstructedBody
{
    public:
    ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity);

    void debugRender();

    void synchronize(std::vector<Velocity> & velocities, std::vector<BodyPosition> & bodyPoses, std::vector<CollisionBox> & collisionBoxes, int index);

    void updateEntity();

    void forceUpdateData(Body * body);

    bool shouldCollide(ConstructedBody & body);

    double mass;
    double invMass;
    double moi;
    double invMoi;

    double friction;
    bool noMaxFriction;

    //non owning pointer to Physics system vector
    Velocity * velocity;
    BodyPosition * bodyPos;
    CollisionBox * box;


    //non owning pointer to Physics system vector
    std::list<CollisionPair>::iterator collisionPairIt;
    std::list<CollisionPair *> contactList;
    std::list<std::list<PivotJointConstraint>::iterator> pivotJointItList;

    BodyType bodyType;
    ShapeType shapeType;
    CollisionType collisionType;
    std::vector<CollisionType> acceptedCollision;

    Body & bodyRef;
    Position & posRef;
    AccelAndSpeed & aasRef;
    aunteater::weak_entity entity;

    friend std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb);
};

//A pair of constructed body colliding. This is persisted between frame
struct CollisionPair
{
    bool cold;

    ConstructedBody & bodyA;
    ConstructedBody & bodyB;
    std::list<CollisionPair *>::iterator iteratorA;
    std::list<CollisionPair *>::iterator iteratorB;

    ContactManifold manifold;
    bool toRemove;

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
    math::Radian<double> angleBaseA;

    //non owning pointer to Physics system vector
    Velocity * velocityB;
    BodyPosition * bodyPosB;
    Vec2 rB;
    double crossB;
    double crossBTangent;
    double invMassB;
    double invMoiB;
    double tangentSpeedB;
    math::Radian<double> angleBaseB;

    double friction;
    bool noMaxFriction;
    double restitution;
    Vec2 normal; 
    Vec2 tangent;

    Vec2 clipPoint;

    ContactFeature & cf;

    void debugRender();

    friend std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc);
};

struct PlayerEnvironmentConstraint
{
    Vec2 normal; 
    double separation;

    //Non owning pointer to the constructed body of the player
    ConstructedBody * cPlayer;
};

struct PivotJointConstraint
{
    explicit PivotJointConstraint(
            double aInvMassA,
            double aInvMoiA,
            Position2 aLocalAnchorA,

            double aInvMassB,
            double aInvMoiB,
            Position2 aLocalAnchorB,

            ConstructedBody * aCbA,
            ConstructedBody * aCbB,

            aunteater::weak_entity aEntity
            ) :
        invMassA{aInvMassA},
        invMoiA{aInvMoiA},
        localAnchorA{aLocalAnchorA},
        invMassB{aInvMassB},
        invMoiB{aInvMoiB},
        localAnchorB{aLocalAnchorB},
        cbA{aCbA},
        cbB{aCbB},
        pivotJointEntity{aEntity}
    {}

    Velocity * velocityA;
    BodyPosition * bodyPosA;
    double invMassA;
    double invMoiA;
    Position2 localAnchorA;
    Vec2 rA = Vec2::Zero();
    Vec2 angVecA = Vec2::Zero();

    Velocity * velocityB;
    BodyPosition * bodyPosB;
    double invMassB;
    double invMoiB;
    Position2 localAnchorB;
    Vec2 rB = Vec2::Zero();
    Vec2 angVecB = Vec2::Zero();

    Vec2 impulse = Vec2::Zero();
    double axialMass;
    math::Matrix<2, 2, double> k = math::Matrix<2, 2>::Zero();

    ConstructedBody * cbA;
    ConstructedBody * cbB;

    aunteater::weak_entity pivotJointEntity;
};
}
}
