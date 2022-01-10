#include "PhysicsStructs.h"

#include "../Configuration.h"

#include "../Components/Body.h"
#include "../Components/PivotJoint.h"
#include "../Components/DistanceJoint.h"
#include "../Components/Position.h"
#include "../Components/WeldJoint.h"

#include "../Utils/CollisionBox.h"
#include "../Utils/DrawDebugStuff.h"
#include "../Utils/PhysicsMathUtilities.h"
#include "Logging.h"
#include "math/Color.h"

#include <math/Vector.h>
#include <iostream>


namespace ad {
namespace grapito {

ConstructedBody::ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity) :
    mass{aBody.mass},
    invMass{aBody.invMass},
    moi{aBody.moi},
    invMoi{aBody.invMoi},
    friction{aBody.friction},
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
            .1f,
            {0,0,255},
    });
    debugDrawer->drawLine({
            cf.contactPoint,
            cf.contactPoint + cf.normalImpulse * normal,
            .1f,
            {150,150,200},
    });

    Vec2 AngVecA = {-rA.y(), rA.x()};
    Vec2 AngVecB = {-rB.y(), rB.x()};
    Vec2 speed = velocityA->v + (AngVecA * velocityA->w) - velocityB->v - (AngVecB * velocityB->w);

    debugDrawer->drawLine({
            cf.contactPoint,
            cf.contactPoint + speed.dot(normal) * normal,
            .1f,
            {0,200,0},
    });
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
    mStiffness{aWeldJoint.mStiffness},
    mDamping{aWeldJoint.mDamping},
    mRefAngle{aBodyB->bodyRef.theta - aBodyA->bodyRef.theta}
{}

void WeldJointConstraint::InitVelocityConstraint(const GrapitoTimer & timer)
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

    math::Matrix<3, 3, float> K = 
    {

        //First row
        mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB, 
        -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB,
        -rA.y() * iA - rB.y() * iB,

        //Second row
        -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB,
        mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB, 
        rA.x() * iA + rB.x() * iB,

        //Third row
        -rA.y() * iA - rB.y() * iB,
        rA.x() * iA + rB.x() * iB,
        iA + iB

    };

    if (mStiffness > 0.f)
    {
        mMassMatrix = getInverse2by2(K);   
        float invM = iA + iB;
        float C = (bodyPosB->a - bodyPosA->a - mRefAngle).value();
        float d = mDamping;
        float k = mStiffness;

        //Apparently this is magic
        mGamma = timer.delta() * (d + timer.delta() * k);
        mGamma = mGamma != 0.f ? 1.f / mGamma : 0.f;
        mBias = C * k * timer.delta() * mGamma;

        invM += mGamma;
        mMassMatrix.at(2,2) = invM != 0.f ? 1.f / invM : 0.f;
    }
    else if (K.at(2,2) == 0.f)
    {
        mMassMatrix = getInverse2by2(K);   
        mGamma = 0.f;
        mBias = 0.f;
    }
    else
    {
        mMassMatrix = K.inverse();
        mGamma = 0.f;
        mBias = 0.f;
    }

    Vec2 impulseSpeed = {mImpulse.x(), mImpulse.y()};

    //warm start constraint
    velocityA->v -= impulseSpeed * invMassA;
    velocityA->w -= invMoiA * (twoDVectorCross(rA, impulseSpeed) + mImpulse.z());
    velocityB->v += impulseSpeed * invMassB;
    velocityB->w += invMoiB * (twoDVectorCross(rB, impulseSpeed) + mImpulse.z());
}

void WeldJointConstraint::SolveVelocityConstraint(const GrapitoTimer &)
{
    float mA = invMassA;
    float iA = invMoiA;
    float mB = invMassB;
    float iB = invMoiB;
    Vec2 vA = velocityA->v;
    Vec2 vB = velocityB->v;
    float wA = velocityA->w;
    float wB = velocityB->w;

    if (mStiffness > 0.f)
    {
        float Cdot2 = wB - wA;

        float impulse2 = -mMassMatrix.at(2, 2) * (Cdot2 + mBias + mGamma * mImpulse.z());
        mImpulse.z() += impulse2;

        wA -= iA * impulse2;
        wB += iB * impulse2;

        Vec2 Cdot1 = vB + wB * angVecB - vA - wA * angVecA;

        Vec2 impulseSpeed = -Cdot1 * mMassMatrix.getSubmatrix(2, 2);
        mImpulse.x() += impulseSpeed.x();
        mImpulse.y() += impulseSpeed.y();

        vA -= mA * impulseSpeed;
        wA -= iA * twoDVectorCross(rA, impulseSpeed);

        vB += mB * impulseSpeed;
        wB += iB * twoDVectorCross(rB, impulseSpeed);
    }
    else
    {
        Vec2 Cdot1 = vB + wB * angVecB - vA - wA * angVecA;
        float Cdot2 = wB - wA;
        Vec3 Cdot{Cdot1.x(), Cdot1.y(), Cdot2};

        Vec3 newImpulse = - Cdot * mMassMatrix;
        mImpulse += newImpulse;

        Vec2 impulseSpeed{newImpulse.x(), newImpulse.y()};

        vA -= mA * impulseSpeed;
        wA -= iA * (twoDVectorCross(rA, impulseSpeed) + newImpulse.z());

        vB += mB * impulseSpeed;
        wB += iB * (twoDVectorCross(rB, impulseSpeed) + newImpulse.z());
    }

    velocityA->v = vA;
    velocityA->w = wA;
    velocityB->v = vB;
    velocityB->w = wB;
}

bool WeldJointConstraint::SolvePositionConstraint()
{
    Position2 pA = bodyPosA->p;
    Position2 pB = bodyPosB->p;
    Position2 cA = bodyPosA->c;
    Position2 cB = bodyPosB->c;
    math::Radian<float> aA = bodyPosA->a;
    math::Radian<float> aB = bodyPosB->a;

    Vec2 rA = transformVector(localAnchorA + pA.as<math::Vec>() - cA.as<math::Position>(), aA);
    Vec2 rB = transformVector(localAnchorB + pB.as<math::Vec>() - cB.as<math::Position>(), aB);

    float mA = invMassA;
    float iA = invMoiA;
    float mB = invMassB;
    float iB = invMoiB;

    math::Matrix<3, 3, float> K = 
    {

        //First row
        mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB, 
        -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB,
        -rA.y() * iA - rB.y() * iB,

        //Second row
        -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB,
        mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB, 
        rA.x() * iA + rB.x() * iB,

        //Third row
        -rA.y() * iA - rB.y() * iB,
        rA.x() * iA + rB.x() * iB,
        iA + iB

    };

    float positionError; 
    float angularError = 0.f; 

    if (mStiffness > 0.f)
    {
        Vec2 C = cB + rB - cA - rA;

        positionError = C.getNorm();

        Vec2 impulse = -solveMatrix(K.getSubmatrix(2, 2), C);

        bodyPosA->c -= impulse * invMassA;
        bodyPosA->p -= impulse * invMassA;

        bodyPosB->c += impulse * invMassB;
        bodyPosB->p += impulse * invMassB;
    }
    else
    {
        Vec2 C1 =  cB + rB - cA - rA;
        float C2 = (aB - aA - mRefAngle).value();

        positionError = C1.getNorm();
        angularError = std::abs(C2);

        Vec3 C{C1.x(), C1.y(), C2};
      
        Vec3 impulse = Vec3::Zero();
        if (K.at(2, 2) > 0.0f)
        {
          impulse = -solveMatrix(K, C);
        }
        else
        {
          Vec2 impulse2 = -solveMatrix(K.getSubmatrix(2, 2), C1);
          impulse = Vec3{impulse2.x(), impulse2.y(), 0.0f};
        }

        Vec2 impulseSpeed{impulse.x(), impulse.y()};

        bodyPosA->c -= mA * impulseSpeed;
        bodyPosA->p -= mA * impulseSpeed;
        bodyPosA->a -= math::Radian<float>(iA * (twoDVectorCross(rA, impulseSpeed) + impulse.z()));

        bodyPosB->c += mB * impulseSpeed;
        bodyPosB->p += mB * impulseSpeed;
        bodyPosB->a += math::Radian<float>(iB * (twoDVectorCross(rB, impulseSpeed) + impulse.z()));
    }

    return positionError <= physic::gLinearSlop && angularError <= physic::gAngularSlop;
}

void WeldJointConstraint::debugRender()
{
    Position2 pA = bodyPosA->p;
    Position2 pB = bodyPosB->p;
    Position2 cA = bodyPosA->c;
    Position2 cB = bodyPosB->c;
    math::Radian<float> aA = bodyPosA->a;
    math::Radian<float> aB = bodyPosB->a;

    Vec2 rA = transformVector(localAnchorA - (cA - pA).as<math::Position>(), aA);
    Vec2 rB = transformVector(localAnchorB - (cB - pB).as<math::Position>(), aB);

    debugDrawer->drawLine({
            cA,
            cA + rA,
            2.f,
            math::sdr::gGreen,
            });
    debugDrawer->drawLine({
            cB,
            cB + rB,
            2.f,
            math::sdr::gGreen,
            });
    debugDrawer->drawCross({
            cA + rA,
            2.f,
            math::sdr::gGreen,
            });
    debugDrawer->drawCross({
            cB + rB,
            2.f,
            math::sdr::gGreen,
            });
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

void PivotJointConstraint::InitVelocityConstraint(const GrapitoTimer &)
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
    velocityA->v -= mImpulse * invMassA;
    velocityA->w -= invMoiA * twoDVectorCross(rA, mImpulse);
    velocityB->v += mImpulse * invMassB;
    velocityB->w += invMoiB * twoDVectorCross(rB, mImpulse);
}

void PivotJointConstraint::SolveVelocityConstraint(const GrapitoTimer &)
{
    Vec2 angularSpeed = velocityB->v + angVecB * velocityB->w -
        velocityA->v - angVecA * velocityA->w;
    Vec2 impulse = solveMatrix(k, -angularSpeed);

    mImpulse += impulse;

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

    return positionError <= physic::gLinearSlop;
}
void PivotJointConstraint::debugRender()
{
    Position2 pA = bodyPosA->p;
    Position2 pB = bodyPosB->p;
    Position2 cA = bodyPosA->c;
    Position2 cB = bodyPosB->c;
    math::Radian<float> aA = bodyPosA->a;
    math::Radian<float> aB = bodyPosB->a;

    Vec2 rA = transformVector(localAnchorA - (cA - pA).as<math::Position>(), aA);
    Vec2 rB = transformVector(localAnchorB - (cB - pB).as<math::Position>(), aB);

    debugDrawer->drawLine({
            cA,
            cA + rA,
            2.f,
            math::sdr::gRed,
            });
    debugDrawer->drawLine({
            cB,
            cB + rB,
            2.f,
            math::sdr::gGreen,
            });
    debugDrawer->drawCross({
            cA + rA,
            0.6f,
            math::sdr::gRed,
            });
    debugDrawer->drawCross({
            cB + rB,
            0.6f,
            math::sdr::gGreen,
            });
}

DistanceJointConstraint::DistanceJointConstraint(
        const DistanceJoint & aDistanceJoint,
        ConstructedBody * aBodyA,
        ConstructedBody * aBodyB,
        aunteater::weak_entity aEntity
        ) :
    JointConstraint{aBodyA, aBodyB, aEntity},
    invMassA{aBodyA->invMass},
    invMoiA{aBodyA->invMoi},
    localAnchorA{aDistanceJoint.localAnchorA},
    invMassB{aBodyB->invMass},
    invMoiB{aBodyB->invMoi},
    localAnchorB{aDistanceJoint.localAnchorB},
    mStiffness{aDistanceJoint.mStiffness},
    mDamping{aDistanceJoint.mDamping}
{
    rA = transformVector(localAnchorA - aBodyA->bodyRef.massCenter, aBodyA->bodyRef.theta);
    rB = transformVector(localAnchorB - aBodyB->bodyRef.massCenter, aBodyB->bodyRef.theta);

    mBaseDiffVector = (aBodyB->bodyRef.massCenter + aBodyB->posRef.position.as<math::Vec>() + rB) - (aBodyA->bodyRef.massCenter + aBodyA->posRef.position.as<math::Vec>() + rA);
    mBaseLength = aDistanceJoint.mLength > 0.f ? aDistanceJoint.mLength : std::max(mBaseDiffVector.getNorm(), physic::gLinearSlop);
    mMinBaseLength = mBaseLength * (1 - aDistanceJoint.mMinSlackFactor); 
    mMaxBaseLength = mBaseLength * (1 + aDistanceJoint.mMaxSlackFactor); 
}

void DistanceJointConstraint::InitVelocityConstraint(const GrapitoTimer & aTimer)
{
    bodyPosA = cbA->bodyPos;
    velocityA = cbA->velocity;
    rA = transformVector(localAnchorA - (bodyPosA->c - bodyPosA->p).as<math::Position>(), bodyPosA->a);
    angVecA = {-rA.y(), rA.x()};

    bodyPosB = cbB->bodyPos;
    velocityB = cbB->velocity;
    rB = transformVector(localAnchorB - (bodyPosB->c - bodyPosB->p).as<math::Position>(), bodyPosB->a);
    angVecB = {-rB.y(), rB.x()};

    mCurrentDirection = (bodyPosB->c + rB) - (bodyPosA->c + rA);
    mCurrentLength = mCurrentDirection.getNorm();

    if (mCurrentLength > physic::gLinearSlop)
    {
        mCurrentDirection *= 1.f / mCurrentLength;
    }
    else
    {
        mCurrentDirection = Vec2::Zero();
        mMass = 0.f;
        mImpulse = 0.f;
        mLowerImpulse = 0.f;
        mUpperImpulse = 0.f;
    }

    float crossRaDirection = twoDVectorCross(rA, mCurrentDirection);
    float crossRbDirection = twoDVectorCross(rB, mCurrentDirection);
    float invMass = invMassA + invMoiA * crossRaDirection* crossRaDirection + invMassB + invMoiB * crossRbDirection * crossRbDirection;

    mMass = invMass != 0.f ? 1.f / invMass : 0.f;

    if (mStiffness > 0.f && mMaxBaseLength > mMinBaseLength)
    {
        float C = std::max(0.f, mCurrentLength - mBaseLength);
        
        float d = mDamping;
        float k = mStiffness;

        mGamma = aTimer.delta() * (d + aTimer.delta() * k);
        mGamma = mGamma != 0.f ? 1.f / mGamma : 0.f;
        mBias = C * aTimer.delta() * k * mGamma;

        invMass += mGamma;
        mSoftMass = invMass != 0.f ? 1.f / invMass : 0.f;
    }
    else
    {
        mGamma = 0.f;
        mBias = 0.f;
        mSoftMass = mMass;
    }

    Vec2 impulse = (mImpulse + mLowerImpulse - mUpperImpulse) * mCurrentDirection;

    //warm start constraint
    velocityA->v -= impulse * invMassA;
    velocityA->w -= invMoiA * twoDVectorCross(rA, impulse);
    velocityB->v += impulse * invMassB;
    velocityB->w += invMoiB * twoDVectorCross(rB, impulse);
}

void DistanceJointConstraint::SolveVelocityConstraint(const GrapitoTimer & aTimer)
{
    float mA = invMassA;
    float iA = invMoiA;
    Vec2 vA = velocityA->v;
    float wA = velocityA->w;

    float mB = invMassB;
    float iB = invMoiB;
    Vec2 vB = velocityB->v;
    float wB = velocityB->w;

    if (mMaxBaseLength > mMinBaseLength)
    {
        if (mStiffness > 0.f && mCurrentLength > mBaseLength)
        {
            Vec2 pointSpeedA = vA + wA * angVecA;
            Vec2 pointSpeedB = vB + wB * angVecB;
            float Cdot = mCurrentDirection.dot(pointSpeedB - pointSpeedA);

            float impulse = -mSoftMass * (Cdot + mBias + mGamma * mImpulse);
            mImpulse += impulse;

            Vec2 impulseVec = impulse * mCurrentDirection;
            vA -= mA * impulseVec;
            wA -= iA * twoDVectorCross(rA, impulseVec);
            vB += mB * impulseVec;
            wB += iB * twoDVectorCross(rB, impulseVec);
        }

        //Lower than min length
        {
            //Scope is just because the computation are really similar
            float C = mCurrentLength - mMinBaseLength;
            float bias = std::max(0.f, C) / aTimer.delta();

            Vec2 pointSpeedA = vA + wA * angVecA;
            Vec2 pointSpeedB = vB + wB * angVecB;
            float Cdot = mCurrentDirection.dot(pointSpeedB - pointSpeedA);

            float impulse = -mMass * (Cdot + bias);
            float oldImpulse = mLowerImpulse;
            mLowerImpulse = std::max(0.f, mLowerImpulse + impulse);
            impulse = mLowerImpulse - oldImpulse;

            Vec2 impulseVec = impulse * mCurrentDirection;
            vA -= mA * impulseVec;
            wA -= iA * twoDVectorCross(rA, impulseVec);
            vB += mB * impulseVec;
            wB += iB * twoDVectorCross(rB, impulseVec);
        }
        //Greater than default length
        {
            //Scope is just because the computation are really similar
            float C = mMaxBaseLength - mCurrentLength;
            float bias = std::max(0.f, C) / aTimer.delta();

            Vec2 pointSpeedA = vA + wA * angVecA;
            Vec2 pointSpeedB = vB + wB * angVecB;
            float Cdot = mCurrentDirection.dot(pointSpeedA - pointSpeedB);

            float impulse = -mMass * (Cdot + bias);
            float oldImpulse = mUpperImpulse;
            mUpperImpulse = std::max(0.f, mUpperImpulse + impulse);
            impulse = mUpperImpulse - oldImpulse;

            Vec2 impulseVec = impulse * mCurrentDirection;
            vA -= mA * impulseVec;
            wA -= iA * twoDVectorCross(rA, impulseVec);
            vB += mB * impulseVec;
            wB += iB * twoDVectorCross(rB, impulseVec);
        }
    }
    else if (mCurrentLength > mBaseLength)
    {
        Vec2 pointSpeedA = vA + wA * angVecA;
        Vec2 pointSpeedB = vB + wB * angVecB;
        float Cdot = mCurrentDirection.dot(pointSpeedB - pointSpeedA);

        float impulse = -mMass * Cdot;
        mImpulse += impulse;

        Vec2 impulseVec = impulse * mCurrentDirection;
        vA -= mA * impulseVec;
        wA -= iA * twoDVectorCross(rA, impulseVec);
        vB += mB * impulseVec;
        wB += iB * twoDVectorCross(rB, impulseVec);
    }

    velocityA->v = vA;
    velocityA->w = wA;
    velocityB->v = vB;
    velocityB->w = wB;
}

bool DistanceJointConstraint::SolvePositionConstraint()
{
    float iA = invMoiA;
    Position2 cA = bodyPosA->c;
    math::Radian<float> aA = bodyPosA->a;

    float iB = invMoiB;
    Position2 cB = bodyPosB->c;
    math::Radian<float> aB = bodyPosB->a;

    rA = transformVector(localAnchorA - (cA - bodyPosA->p).as<math::Position>(), aA);
    rB = transformVector(localAnchorB - (cB - bodyPosB->p).as<math::Position>(), aB);

    mCurrentDirection = (cB + rB) - (cA + rA);
    float length = mCurrentDirection.getNorm();
    mCurrentDirection /= length;
    float C;

    if (mMaxBaseLength == mMinBaseLength)
    {
        C = length - mMinBaseLength;
    }
    else if (length < mMinBaseLength)
    {
        C = length - mMinBaseLength;
    }
    else if (length > mMaxBaseLength)
    {
        C = length - mMaxBaseLength;
    }
    else
    {
        //Distance is correct, no need to adjust
        return true;
    }

    float impulse = -mMass * C;
    Vec2 impulseVec = impulse * mCurrentDirection;
    bodyPosA->c -= impulseVec * invMassA;
    bodyPosA->p -= impulseVec * invMassA;
    bodyPosA->a -= static_cast<math::Radian<float>>(iA * twoDVectorCross(rA, impulseVec));
    bodyPosB->c += impulseVec * invMassB;
    bodyPosB->p += impulseVec * invMassB;
    bodyPosB->a += static_cast<math::Radian<float>>(iB * twoDVectorCross(rB, impulseVec));

    return std::abs(C) < physic::gLinearSlop;
}

void DistanceJointConstraint::debugRender()
{
    Position2 cA = bodyPosA->c;
    math::Radian<float> aA = bodyPosA->a;

    Position2 cB = bodyPosB->c;
    math::Radian<float> aB = bodyPosB->a;

    rA = transformVector(localAnchorA - (cA - bodyPosA->p).as<math::Position>(), aA);
    rB = transformVector(localAnchorB - (cB - bodyPosB->p).as<math::Position>(), aB);

    Vec2 distanceVec = (cA + rA - (cB + rB)).normalize() * mBaseLength;

    debugDrawer->drawLine({cB + rB, cB + rB + distanceVec, 2.f, math::sdr::gMagenta});
}

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
