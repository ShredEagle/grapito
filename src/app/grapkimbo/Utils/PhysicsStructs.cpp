#include "PhysicsStructs.h"
#include "Components/Body.h"
#include "Components/PivotJoint.h"
#include "Components/Position.h"

#include "Components/WeldJoint.h"
#include "Configuration.h"
#include "Utils/CollisionBox.h"
#include "Utils/DrawDebugStuff.h"
#include "math/Vector.h"
#include <iostream>


namespace ad {
namespace grapito {

ConstructedBody::ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity) :
    mass{aBody.mass},
    invMass{aBody.invMass},
    moi{aBody.moi},
    invMoi{aBody.invMoi},
    friction{aBody.friction},
    noMaxFriction{aBody.noMaxFriction},
    bodyType{aBody.bodyType},
    shapeType{aBody.shapeType},
    collisionType{aBody.collisionType},
    acceptedCollision{aBody.acceptedCollision},
    bodyRef{aBody},
    posRef{aPos},
    aasRef{aAas},
    entity{aEntity}
{
}

void ConstructedBody::forceUpdateData(Body * body)
{
    mass = body->mass;
    invMass = body->invMass;
    moi = body->moi;
    invMoi = body->invMoi;
}

void ConstructedBody::synchronize(
        std::vector<Velocity> & velocities,
        std::vector<BodyPosition> & bodyPoses,
        std::vector<CollisionBox> & collisionBoxes,
        int index
)
{
    box = &collisionBoxes.at(index);
    velocity = &velocities.at(index);
    bodyPos = &bodyPoses.at(index);
}

void ConstructedBody::updateEntity()
{
    bodyRef.theta = bodyPos->a;
    aasRef.speed = velocity->v;
    aasRef.w = velocity->w;
    posRef.position = bodyPos->p;
}

bool ConstructedBody::shouldCollide(ConstructedBody & body)
{
    bool result = true;
    result = (result && std::find(body.acceptedCollision.begin(), body.acceptedCollision.end(), collisionType) != body.acceptedCollision.end()) || body.acceptedCollision.size() == 0;
    result = (result && std::find(acceptedCollision.begin(), acceptedCollision.end(), body.collisionType) != acceptedCollision.end()) || acceptedCollision.size() == 0;
    return result;
}

void ConstructedBody::debugRender()
{
    for (size_t i = 0; i < box->shape.mFaceCount; ++i)
    {
        auto vertex = box->shape.getVertice(i);
        debugDrawer->drawPoint({
                vertex,
                .05f,
                {255,255,0},
        });
    }
    debugDrawer->drawPoint({
            bodyPos->c,
            .05f,
            {255,100,0},
    });
}

void CollisionPair::debugRender()
{
    debugDrawer->drawLine({
            bodyA.bodyPos->c,
            bodyB.bodyPos->c,
            0.,
            {255,0,255},
    });
}

void VelocityConstraint::debugRender()
{
    debugDrawer->drawPoint({
            cf.contactPoint,
            .05,
            {0,0,255},
    });
    /*
    debugDrawer->drawLine({
            cf.contactPoint,
            cf.contactPoint + normal * 3,
            .05,
            {255,0,0},
    });
    debugDrawer->drawLine({
            cf.contactPoint,
            cf.contactPoint + cf.normalImpulse * normal * 3,
            .05,
            {150,150,200},
    });

    Vec2 AngVecA = {-rA.y(), rA.x()};
    Vec2 AngVecB = {-rB.y(), rB.x()};
    Vec2 speed = velocityA->v + (AngVecA * velocityA->w) - velocityB->v - (AngVecB * velocityB->w);

    debugDrawer->drawLine({
            cf.contactPoint,
            cf.contactPoint + speed.dot(normal) * normal,
            .05,
            {0,200,0},
    });
    */
}

WeldJointConstraint::WeldJointConstraint(
        const WeldJoint & aWeldJoint,
        ConstructedBody * aBodyA,
        ConstructedBody * aBodyB,
        aunteater::weak_entity aEntity
        ) :
    JointConstraint{aBodyA, aBodyB, aEntity},
    invMassA{aBodyA->invMass},
    invMoiA{aBodyA->invMoi},
    localAnchorA{aWeldJoint.localAnchorA},
    invMassB{aBodyB->invMass},
    invMoiB{aBodyB->invMoi},
    localAnchorB{aWeldJoint.localAnchorB},
    mRefAngle{aBodyA->bodyPos->a - aBodyB->bodyPos->a},
    mStiffness{aWeldJoint.mStiffness},
    mDamping{aWeldJoint.mDamping}
{}

void WeldJointConstraint::InitVelocityConstraint()
{}

void WeldJointConstraint::SolveVelocityConstraint()
{}

bool WeldJointConstraint::SolvePositionConstraint()
{
    return true;
}

void PivotJointConstraint::InitVelocityConstraint()
{
    bodyPosA = cbA->bodyPos;
    bodyPosB = cbB->bodyPos;
    velocityA = cbA->velocity;
    velocityB = cbB->velocity;
    rA = transformVector(localAnchorA - (bodyPosA->c - bodyPosA->p).as<math::Position>(), bodyPosA->a);
    rB = transformVector(localAnchorB - (bodyPosB->c - bodyPosB->p).as<math::Position>(), bodyPosB->a);
    angVecA = {-rA.y(), rA.x()};
    angVecB = {-rB.y(), rB.x()};

    float mA = invMassA;
    float iA = invMoiA;
    float mB = invMassB;
    float iB = invMoiB;

    k.at(0,0) = mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB;
    k.at(0,1) = -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB;
    k.at(1,0) = k.at(0,1);
    k.at(1,1) = mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB;

    //warm start constraint
    velocityA->v -= impulse * invMassA;
    velocityA->w -= invMoiA * twoDVectorCross(rA, impulse);
    velocityB->v += impulse * invMassB;
    velocityB->w += invMoiB * twoDVectorCross(rB, impulse);
}

void PivotJointConstraint::SolveVelocityConstraint()
{
    Vec2 angularSpeed = velocityB->v + angVecB * velocityB->w -
        velocityA->v - angVecA * velocityA->w;
    Vec2 impulse = solveMatrix(k, -angularSpeed);

    impulse += impulse;

    velocityA->v -= impulse * invMassA;
    velocityA->w -= invMoiA * twoDVectorCross(rA, impulse);
    velocityB->v += impulse * invMassB;
    velocityB->w += invMoiB * twoDVectorCross(rB, impulse);
}

bool PivotJointConstraint::SolvePositionConstraint()
{
        Vec2 rA = transformVector(localAnchorA - (bodyPosA->c - bodyPosA->p).as<math::Position>(), bodyPosA->a);
        Vec2 rB = transformVector(localAnchorB - (bodyPosB->c - bodyPosB->p).as<math::Position>(), bodyPosB->a);

        Vec2 C = bodyPosB->c + rB - bodyPosA->c - rA;
        float positionError = C.getNorm();

        float mA = invMassA;
        float iA = invMoiA;
        float mB = invMassB;
        float iB = invMoiB;

        k.at(0,0) = mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB;
        k.at(0,1) = -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB;
        k.at(1,0) = k.at(0,1);
        k.at(1,1) = mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB;

        Vec2 impulse = solveMatrix(-k, C);

        bodyPosA->c -= mA * impulse;
        bodyPosA->p -= mA * impulse;
        bodyPosA->a -= math::Radian<float>{iA * twoDVectorCross(rA, impulse)};

        bodyPosB->c += mB * impulse;
        bodyPosB->p += mB * impulse;
        bodyPosB->a += math::Radian<float>{iB * twoDVectorCross(rB, impulse)};

        return positionError <= physic::linearSlop;
}

PivotJointConstraint::PivotJointConstraint(
        const PivotJoint & aPivotJoint,
        ConstructedBody * aBodyA,
        ConstructedBody * aBodyB,
        aunteater::weak_entity aEntity
        ) :
    JointConstraint{aBodyA, aBodyB, aEntity},
    invMassA{aBodyA->invMass},
    invMoiA{aBodyA->invMoi},
    localAnchorA{aPivotJoint.localAnchorA},
    invMassB{aBodyB->invMass},
    invMoiB{aBodyB->invMoi},
    localAnchorB{aPivotJoint.localAnchorB}
{}

std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc)
{
    os << "{ VelocityConstraint\n";
    os << "    contact : {\n";
    os << "        x : " << vc.cf.contactPoint.x() << "\n";
    os << "        y : " << vc.cf.contactPoint.y() << "\n";
    os << "        nI : " << vc.cf.normalImpulse << "\n";
    os << "        tI : " << vc.cf.tangentImpulse << "\n";
    os << "    }\n";
    os << "    normal : {\n";
    os << "        x : " << vc.normal.x() << "\n";
    os << "        y : " << vc.normal.y() << "\n";
    os << "    }\n";
    os << "    position A : {\n";
    os << "        x : " << vc.bodyPosA->p.x() << "\n";
    os << "        y : " << vc.bodyPosA->p.y() << "\n";
    os << "    }\n";
    os << "    position B : {\n";
    os << "        x : " << vc.bodyPosB->p.x() << "\n";
    os << "        y : " << vc.bodyPosB->p.y() << "\n";
    os << "    }\n";
    os << "    vA : " << vc.velocityA->v << "\n";
    os << "    vB : " << vc.velocityB->v << "\n";
    os << "    wA : " << vc.velocityA->w << "\n";
    os << "    wB : " << vc.velocityB->w << "\n";
    os << "    invMassA : " << vc.invMassA << "\n";
    os << "    invMassB : " << vc.invMassB << "\n";
    os << "    invMoiA : " << vc.invMoiA << "\n";
    os << "    invMoiB : " << vc.invMoiB << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb)
{
    os << "{ ConstructedBody\n";
    os << "    x : " << cb.bodyPos->p.x() << "\n";
    os << "    y : " << cb.bodyPos->p.y() << "\n";
    os << "    speed : " << cb.velocity->v << "\n";
    os << "    w : " << cb.velocity->w << "\n";
    os << "    bodyType : " << cb.bodyType << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ContactManifold & cm)
{
    os << "{ ContactManifold\n";
    os << "    Face : " << cm.face << "\n";
    os << "    refIndex : " << cm.referenceEdgeIndex << "\n";
    os << "    incIndex : " << cm.incidentEdgeIndex << "\n";
    os << "    separation : " << cm.separation << "\n";
    os << "    normal : " << cm.normal << "\n";
    os << "    contacts # : " << cm.contacts.size() << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ContactFeature & cm)
{
    os << "{ ContactFeature\n";
    os << "    refIndex : " << unsigned(cm.indexReference) << "\n";
    os << "    incIndex : " << unsigned(cm.indexIncident) << "\n";
    os << "    typeReference : " << unsigned(cm.typeReference) << "\n";
    os << "    typeIncident : " << unsigned(cm.typeIncident) << "\n";
    os << "}\n";
    return os;
}
}
}
