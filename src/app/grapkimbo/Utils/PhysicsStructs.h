#pragma once

#include "../commons.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/Position.h"

#include "../Utils/CollisionBox.h"

#include <math/Angle.h>
#include <math/Matrix.h>

namespace ad {
namespace grapito {

struct WeldJoint;
struct PivotJoint;
struct DistanceJoint;
struct Body;
struct CollisionPair;
class JointConstraint;

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
    CollisionType_NoCollisionType,
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
    float normalImpulse = 0.f;
    float tangentImpulse = 0.f;
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
    float separation = -std::numeric_limits<float>::max();

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
    explicit Velocity(Vec2 aV, float aW) :
        v{aV},
        w{aW}
    {}

    Vec2 v;
    float w;
};

//Same as velocity but for the position of a body
struct BodyPosition
{
    explicit BodyPosition(Position2 aP, Position2 aC, math::Radian<float> aA) :
        p{aP},
        c{aC},
        a{aA}
    {}

    Position2 p;
    Position2 c;
    math::Radian<float> a;
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

    float mass;
    float invMass;
    float moi;
    float invMoi;

    float friction;

    //non owning pointer to Physics system vector
    Velocity * velocity;
    BodyPosition * bodyPos;
    CollisionBox * box;

    //The list of contacts the body is involved in
    //This is used for impulse persistence
    std::list<CollisionPair *> contactList;

    //Those iterator are used for cleanup
    std::list<CollisionPair>::iterator collisionPairIt;
    // this should probably use some sort of polymorphism
    // for this we would need the list of joint to be pointer based
    // this has some issue vis-a-vis the memory coherence of the physics system
    std::list<std::list<std::unique_ptr<JointConstraint>>::iterator> jointItList;

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
    Vec2 angVecA;
    float crossA;
    float crossASquared;
    float crossATangent;
    float crossATangentSquared;
    float invMassA;
    float invMoiA;
    math::Radian<float> angleBaseA;

    //non owning pointer to Physics system vector
    Velocity * velocityB;
    BodyPosition * bodyPosB;
    Vec2 rB;
    Vec2 angVecB;
    float crossB;
    float crossBSquared;
    float crossBTangent;
    float crossBTangentSquared;
    float invMassB;
    float invMoiB;
    math::Radian<float> angleBaseB;

    float totalMass;
    float totalNormalAngularMass;
    float totalTangentAngularMass;
    float friction;
    float restitution;
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
    float separation;

    //Non owning pointer to the constructed body of the player
    ConstructedBody * cPlayer;
};

class JointConstraint
{
    public:
    JointConstraint(
            ConstructedBody * aBodyA,
            ConstructedBody * aBodyB,
            aunteater::weak_entity aEntity
            ) :
        cbA{aBodyA},
        cbB{aBodyB},
        jointEntity{aEntity}
    {}
    virtual ~JointConstraint() = default;

    virtual void InitVelocityConstraint(const GrapitoTimer & aTimer) = 0;
    virtual void SolveVelocityConstraint(const GrapitoTimer & aTimer) = 0;
    virtual bool SolvePositionConstraint() = 0;

    ConstructedBody * cbA;
    ConstructedBody * cbB;

    aunteater::weak_entity jointEntity;
};

class WeldJointConstraint : public JointConstraint
{
    public:
    WeldJointConstraint(
            const WeldJoint & aWeldJoint,
            ConstructedBody * aBodyA,
            ConstructedBody * aBodyB,
            aunteater::weak_entity aEntity
            );

    void InitVelocityConstraint(const GrapitoTimer & aTimer) override;
    void SolveVelocityConstraint(const GrapitoTimer & aTimer) override;
    bool SolvePositionConstraint() override;

    protected:
    Velocity * velocityA;
    BodyPosition * bodyPosA;
    float invMassA;
    float invMoiA;
    Position2 localAnchorA;
    Vec2 rA = Vec2::Zero();
    Vec2 angVecA = Vec2::Zero();

    Velocity * velocityB;
    BodyPosition * bodyPosB;
    float invMassB;
    float invMoiB;
    Position2 localAnchorB;
    Vec2 rB = Vec2::Zero();
    Vec2 angVecB = Vec2::Zero();

    Vec3 mImpulse = Vec3::Zero();
    math::Matrix<3, 3, float> mMassMatrix = math::Matrix<3, 3, float>::Zero();
    float mGamma;
    float mBias;
    math::Radian<float> mRefAngle;
    float mStiffness;
    float mDamping;
};

class PivotJointConstraint : public JointConstraint
{
    public:
    PivotJointConstraint(
            const PivotJoint & aPivotJoint,
            ConstructedBody * aBodyA,
            ConstructedBody * aBodyB,
            aunteater::weak_entity aEntity
            );

    void InitVelocityConstraint(const GrapitoTimer & aTimer) override;
    void SolveVelocityConstraint(const GrapitoTimer & aTimer) override;
    bool SolvePositionConstraint() override;

    protected:
    Velocity * velocityA;
    BodyPosition * bodyPosA;
    float invMassA;
    float invMoiA;
    Position2 localAnchorA;
    Vec2 rA = Vec2::Zero();
    Vec2 angVecA = Vec2::Zero();

    Velocity * velocityB;
    BodyPosition * bodyPosB;
    float invMassB;
    float invMoiB;
    Position2 localAnchorB;
    Vec2 rB = Vec2::Zero();
    Vec2 angVecB = Vec2::Zero();

    Vec2 mImpulse = Vec2::Zero();
    float axialMass;
    math::Matrix<2, 2, float> k = math::Matrix<2, 2, float>::Zero();

    friend std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc);
};

class DistanceJointConstraint : public JointConstraint
{
    public:
    DistanceJointConstraint(
            const DistanceJoint & aDistanceJoint,
            ConstructedBody * aBodyA,
            ConstructedBody * aBodyB,
            aunteater::weak_entity aEntity
            );

    void InitVelocityConstraint(const GrapitoTimer & aTimer) override;
    void SolveVelocityConstraint(const GrapitoTimer & aTimer) override;
    bool SolvePositionConstraint() override;

    protected:
    Velocity * velocityA;
    BodyPosition * bodyPosA;
    float invMassA;
    float invMoiA;
    Position2 localAnchorA;
    Vec2 rA = Vec2::Zero();
    Vec2 angVecA = Vec2::Zero();

    Velocity * velocityB;
    BodyPosition * bodyPosB;
    float invMassB;
    float invMoiB;
    Position2 localAnchorB;
    Vec2 rB = Vec2::Zero();
    Vec2 angVecB = Vec2::Zero();

    float mStiffness = 0.f;
    float mDamping = 0.f;
    float mMass = 0.f;
    float mSoftMass = 0.f;
    float mGamma = 0.f;
    float mBias = 0.f;
    float mImpulse = 0.f;
    float mLowerImpulse = 0.f;
    float mUpperImpulse = 0.f;
    Vec2 mBaseDiffVector = Vec2::Zero();
    Vec2 mCurrentDirection = Vec2::Zero();
    float mBaseLength = 0.f;
    float mMinBaseLength = 0.f;
    float mMaxBaseLength = 0.f;
    float mCurrentLength = 0.f;
};
}
}
