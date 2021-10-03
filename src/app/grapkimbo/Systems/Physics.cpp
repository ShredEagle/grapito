#include "Physics.h"

#include "Configuration.h"

#include "Components/Body.h"
#include "Components/PivotJoint.h"
#include "Components/PlayerData.h"

#include "Utils/HomogeneousTransformation.h"
#include "Utils/PhysicsStructs.h"
#include "math/Angle.h"
#include "math/Interpolation.h"

#include <aunteater/Timer.h>

#include <math/Matrix.h>

#include <iostream>

namespace ad {
namespace grapito
{

BodyObserver::BodyObserver(Physics * aPhysicsSystem) :
    mPhysicsSystem{aPhysicsSystem}
{}

void BodyObserver::addedEntity(aunteater::LiveEntity & aEntity)
{
    ConstructedBody body = ConstructedBody{aEntity.get<Body>(), aEntity.get<Position>(), aEntity.get<AccelAndSpeed>(), &aEntity};
    aEntity.get<Body>().constructedBodyIt =
        mPhysicsSystem->constructedBodies.insert(
                mPhysicsSystem->constructedBodies.end(),
                body);

}

void BodyObserver::removedEntity(aunteater::LiveEntity & aEntity)
{
    ConstructedBody & body = *aEntity.get<Body>().constructedBodyIt;

    auto collisionPairIt = body.contactList.begin();
    //Removing collision pairs involving this body
    while (collisionPairIt != body.contactList.end())
    {
        auto collisionPair = *collisionPairIt;
        if (collisionPair == *collisionPair->iteratorA)
        {
            collisionPairIt = collisionPair->bodyA.contactList.erase(
                    collisionPair->iteratorA
                    );
            collisionPair->bodyB.contactList.erase(
                    collisionPair->iteratorB
                    );
        }
        else
        {
            collisionPair->bodyA.contactList.erase(
                    collisionPair->iteratorA
                    );
            collisionPairIt = collisionPair->bodyB.contactList.erase(
                    collisionPair->iteratorB
                    );
        }

        collisionPair->toRemove = true;
    }

    for (auto pjcIt : body.pivotJointItList)
    {
        PivotJointConstraint & pjc = *pjcIt;
        pjc.pivotJointEntity->markToRemove();
    }

    mPhysicsSystem->constructedBodies.erase(aEntity.get<Body>().constructedBodyIt);
    //TODO(franz): Need to delete collisionPair also
}

PivotObserver::PivotObserver(Physics * aPhysicsSystem) :
    mPhysicsSystem{aPhysicsSystem}
{}

void PivotObserver::addedEntity(aunteater::LiveEntity & aEntity)
{
    //assert(!aEntity.get<PivotJoint>().bodyA->has<Body>() && "Pivot jointed entity A must have a body component");
    //assert(!aEntity.get<PivotJoint>().bodyB->has<Body>() && "Pivot jointed entity B must have a body component");
    PivotJoint & pivotJoint = aEntity.get<PivotJoint>();
    ConstructedBody & bodyA = *pivotJoint.bodyA->get<Body>().constructedBodyIt;
    ConstructedBody & bodyB = *pivotJoint.bodyB->get<Body>().constructedBodyIt;

    auto pivotJointIt = mPhysicsSystem->pivotJointConstraints.insert(
        mPhysicsSystem->pivotJointConstraints.end(),
        PivotJointConstraint{
             bodyA.invMass,
             bodyA.invMoi,
             pivotJoint.localAnchorA,

             bodyB.invMass,
             bodyB.invMoi,
             pivotJoint.localAnchorB,

             &bodyA,
             &bodyB,

             &aEntity,
        }
    );

    auto itA = bodyA.pivotJointItList.insert(
            bodyA.pivotJointItList.end(),
            pivotJointIt
    );
    auto itB = bodyB.pivotJointItList.insert(
            bodyB.pivotJointItList.end(),
            pivotJointIt
    );

    aEntity.get<PivotJoint>().constraintIt = pivotJointIt;
    aEntity.get<PivotJoint>().constructedBodyConstraintItB = itA;
    aEntity.get<PivotJoint>().constructedBodyConstraintItA = itB;
}

void PivotObserver::removedEntity(aunteater::LiveEntity & aEntity)
{
    PivotJoint & pivot = aEntity.get<PivotJoint>();
    mPhysicsSystem->pivotJointConstraints.erase(pivot.constraintIt);

    if (aEntity.get<PivotJoint>().bodyA->has<Body>())
    {
        ConstructedBody & bodyA = *aEntity.get<PivotJoint>().bodyA->get<Body>().constructedBodyIt;
        bodyA.pivotJointItList.erase(pivot.constructedBodyConstraintItA);
    }
    if (aEntity.get<PivotJoint>().bodyB->has<Body>())
    {
        ConstructedBody & bodyB = *aEntity.get<PivotJoint>().bodyB->get<Body>().constructedBodyIt;
        bodyB.pivotJointItList.erase(pivot.constructedBodyConstraintItB);
    }

}

static inline bool getBestQuery(
    ContactManifold & resultManifold,
    const ConstructedBody & bodyA,
    const ConstructedBody & bodyB
)
{
    ContactManifold queryA = queryFunctions[bodyA.shapeType][bodyB.shapeType](bodyA, bodyB);


    if (queryA.separation > 0.f)
    {
        return false;
    }

    ContactManifold queryB = queryFunctions[bodyB.shapeType][bodyA.shapeType](bodyB, bodyA);

    if (queryB.separation > 0.f)
    {
        return false;
    }
    
    // We want the highest distance to minimize displacement because those distance are
    // negative !
    if (queryB.separation > queryA.separation + 0.001)
    {
        resultManifold = queryB;
        resultManifold.face = ContactManifold::FACEB;
    }
    else 
    {
        resultManifold = queryA;
        resultManifold.face = ContactManifold::FACEA;
    }

    return true;
}

static inline int findIncidentEdge(
    Vec2 edgeDirection,
    const Shape & incidentShape
)
{
    double bestDot = std::numeric_limits<double>::max();
    int incidentIndex;

    // We want to find the most antiparallel edge to our referenceEdge
    // The edgeDirection direction is reversed according to the referenceEdge
    // (this is to get a correctly oriented reference frame)
    // so we compute the dot of all incident edges with the edgeDirection 
    // and we store the maximum
    for (int i = 0; i < incidentShape.mFaceCount; ++i)
    {
        Shape::Edge edge = incidentShape.getEdge(i);
        
        double dot = edgeDirection.dot(edge.end - edge.origin);

        if (dot < bestDot)
        {
            bestDot = dot;
            incidentIndex = i;
        }
    }

    // If the dot is 0 or more this means the two faces are either
    // orthogonal or back facing
    if (bestDot >= 0.)
        return -1;

    return incidentIndex;
}

static inline std::vector<ContactFeature> CheckContactValidity(const Shape::Edge & referenceEdge, std::array<ContactFeature, 2> points)
{
    std::vector<ContactFeature> result;

    double separation = std::numeric_limits<double>::max();

    separation = std::min(separation, referenceEdge.origin.as<math::Vec>().dot(referenceEdge.normal));
    separation = std::min(separation, referenceEdge.end.as<math::Vec>().dot(referenceEdge.normal));

    for (auto contact : points)
    {
        double candidateSeparation = contact.contactPoint.as<math::Vec>().dot(referenceEdge.normal);

        //This is sad that we get fucked by smaller than difference 0.01mm
        if (
                candidateSeparation < separation + physic::linearSlop
           )
        {
            result.emplace_back(contact);
        }
    }

    return result;
}

// This can only be used if we know lineA and lineB
// intersect
// see https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// where lineA is P->P+R and lineB is Q->Q+S
static inline bool findIntersectionPoint(Position2 & result, const Line lineA, const Line lineB)
{
    double rCrossS = twoDVectorCross(lineA.direction, lineB.direction);

    Vec2 base = lineB.origin - lineA.origin;
    double t = twoDVectorCross(base, lineB.direction) / rCrossS;
    double u = twoDVectorCross(base, lineA.direction) / rCrossS;

    if (t < 0. || t > 1. || u < 0. || u > 1)
    {
        return false;
    }

    result = lineA.origin + t * lineA.direction;

    return true;
}


static inline std::vector<ContactFeature> getContactPoints(
    const uint8_t referenceEdgeIndex,
    const uint8_t incidentEdgeIndex,
    Shape & refShape,
    Shape & incShape
)
{
    std::vector<ContactFeature> candidateContact;
    Shape::Edge referenceEdge = refShape.getEdge(referenceEdgeIndex);
    Shape::Edge incidentEdge = incShape.getEdge(incidentEdgeIndex);
    // TODO we should test and take into account the already found contactPoint query.contacts

    Line lineA{referenceEdge.origin, -referenceEdge.normal * 20.};
    Line lineB{referenceEdge.end, -referenceEdge.normal * 20.};

    // We then find all intersecting point between the clipping lines and the
    // best incident edge
    Position2 intersectA{0., 0.};
    Position2 intersectB{0., 0.};
    Line incidentLine{incidentEdge.origin, incidentEdge.end - incidentEdge.origin};

    bool hasInterA = findIntersectionPoint(intersectA, lineA, incidentLine);
    bool hasInterB = findIntersectionPoint(intersectB, lineB, incidentLine);

    std::array<ContactFeature, 2> candidate{};

    if (hasInterA)
    {
        auto contactFeature = ContactFeature{
            ContactFeature::vertex,
            ContactFeature::face,
            referenceEdgeIndex,
            static_cast<uint8_t>((incidentEdgeIndex + 1) % incShape.mFaceCount),
            intersectA
        };
        candidate[0] = contactFeature;
    }
    else 
    {
        // If the line passing by referenceEdge.origin does not clip the incidentEdge
        // it's the end of the incidentEdge that will possibly be inside
        // the referenceBox
        // This is because they are antiparallel
        auto contactFeature = ContactFeature{
            ContactFeature::vertex,
            ContactFeature::face,
            referenceEdgeIndex,
            static_cast<uint8_t>((incidentEdgeIndex + 1) % incShape.mFaceCount),
            incidentEdge.end,
        };

        candidate[0] = contactFeature;
    }

    if (hasInterB)
    {
        auto contactFeature = ContactFeature{
            ContactFeature::vertex,
            ContactFeature::face,
            static_cast<uint8_t>((referenceEdgeIndex + 1) % refShape.mFaceCount),
            incidentEdgeIndex,
            intersectB
        };

        candidate[1] = contactFeature;
    }
    else
    {
        // If the line passing by referenceEdge.end does not clip the incidentEdge
        // it's the origin of the incidentEdge that will possibly be inside
        // the referenceBox
        // This is because they are antiparallel
        auto contactFeature = ContactFeature{
            ContactFeature::vertex,
            ContactFeature::face,
            static_cast<uint8_t>((referenceEdgeIndex + 1) % refShape.mFaceCount),
            incidentEdgeIndex,
            incidentEdge.origin,
        };
        candidate[1] = contactFeature;
    }

    auto validContacts = CheckContactValidity(referenceEdge, candidate);

    return validContacts;
}

static std::vector<Position2> createTransformedCollisionBox(Body & aBody, Position & aPos)
{
    std::vector<Position2> transformedVertices;

    for (auto vertex : aBody.shape.vertices)
    {
        auto transformedPos = transformPosition(
                static_cast<Position2>(vertex.as<math::Vec>() + aPos.position.as<math::Vec>()),
                aBody.theta,
                static_cast<Position2>(aBody.massCenter.as<math::Vec>() + aPos.position.as<math::Vec>())
                );
        transformedVertices.emplace_back(transformedPos);
    }

    return transformedVertices;
}

static inline void solveContactVelocityConstraint(VelocityConstraint & constraint, const aunteater::Timer & aTimer)
{

    ContactFeature & cf = constraint.cf;
    Vec2 AngVecA = {-constraint.rA.y(), constraint.rA.x()};
    Vec2 AngVecB = {-constraint.rB.y(), constraint.rB.x()};


    double totalMass = constraint.invMassA + constraint.invMassB;


    double crossASquared = constraint.crossA * constraint.crossA;
    double crossBSquared = constraint.crossB * constraint.crossB;
    double totalAngularMass = constraint.invMoiA * crossASquared + constraint.invMoiB * crossBSquared;

    if (constraint.friction > 0.)
    {
        Vec2 speed = constraint.velocityA->v + (AngVecA * constraint.velocityA->w) - constraint.velocityB->v - (AngVecB * constraint.velocityB->w);

        double tangentSpeed = speed.dot(constraint.tangent);

        double crossATangentSquared = constraint.crossATangent * constraint.crossATangent;
        double crossBTangentSquared = constraint.crossBTangent * constraint.crossBTangent;

        double totalAngularTangentMass = constraint.invMoiA * crossATangentSquared + constraint.invMoiB * crossBTangentSquared;

        double lambda = -(1 / (totalMass + totalAngularTangentMass)) * (tangentSpeed);
        double maxFriction = constraint.friction * cf.normalImpulse;
        double newImpulseTangent = std::max(std::min(cf.tangentImpulse + lambda, maxFriction), -maxFriction);
        lambda = newImpulseTangent - cf.tangentImpulse;
        cf.tangentImpulse = newImpulseTangent;


        Vec2 impulse = lambda * constraint.tangent;
        double angularImpulseA = lambda * constraint.crossATangent * constraint.invMoiA;
        double angularImpulseB = lambda * constraint.crossBTangent * constraint.invMoiB;

        constraint.velocityA->v += impulse * constraint.invMassA;
        constraint.velocityA->w += angularImpulseA;
        constraint.velocityB->v -= impulse * constraint.invMassB;
        constraint.velocityB->w -= angularImpulseB;
    }

    Vec2 speed = constraint.velocityA->v + (AngVecA * constraint.velocityA->w) - constraint.velocityB->v - (AngVecB * constraint.velocityB->w);
    double normalSpeed = speed.dot(constraint.normal);

    double lambda = -(1 / (totalMass + totalAngularMass)) * (normalSpeed);
    double newImpulse = std::max(cf.normalImpulse + lambda, 0.);
    lambda = newImpulse - cf.normalImpulse;
    cf.normalImpulse = newImpulse;

    Vec2 impulse = lambda * constraint.normal;
    double angularImpulseA = lambda * constraint.crossA * constraint.invMoiA;

    double angularImpulseB = lambda * constraint.crossB * constraint.invMoiB;

    constraint.velocityA->v += impulse * constraint.invMassA;
    constraint.velocityA->w += angularImpulseA;
    constraint.velocityB->v -= impulse * constraint.invMassB;
    constraint.velocityB->w -= angularImpulseB;
}

static inline void solvePivotJointVelocityConstraint(PivotJointConstraint & constraint, const aunteater::Timer & aTimer)
{
    Vec2 angularSpeed = constraint.velocityB->v + constraint.angVecB * constraint.velocityB->w -
        constraint.velocityA->v - constraint.angVecA * constraint.velocityA->w;
    Vec2 impulse = solveMatrix(constraint.k, -angularSpeed);

    constraint.impulse += impulse;

    constraint.velocityA->v -= impulse * constraint.invMassA;
    constraint.velocityA->w -= constraint.invMoiA * twoDVectorCross(constraint.rA, impulse);
    constraint.velocityB->v += impulse * constraint.invMassB;
    constraint.velocityB->w += constraint.invMoiB * twoDVectorCross(constraint.rB, impulse);
}

static inline bool solvePivotJointPositionConstraint(PivotJointConstraint & constraint)
{
        Vec2 rA = transformVector(constraint.localAnchorA - (constraint.bodyPosA->c - constraint.bodyPosA->p).as<math::Position>(), constraint.bodyPosA->a);
        Vec2 rB = transformVector(constraint.localAnchorB - (constraint.bodyPosB->c - constraint.bodyPosB->p).as<math::Position>(), constraint.bodyPosB->a);

        Vec2 C = constraint.bodyPosB->c + rB - constraint.bodyPosA->c - rA;
        double positionError = C.getNorm();

        double mA = constraint.invMassA;
        double iA = constraint.invMoiA;
        double mB = constraint.invMassB;
        double iB = constraint.invMoiB;

        constraint.k.at(0,0) = mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB;
        constraint.k.at(0,1) = -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB;
        constraint.k.at(1,0) = constraint.k.at(0,1);
        constraint.k.at(1,1) = mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB;

        Vec2 impulse = solveMatrix(-constraint.k, C);

        constraint.bodyPosA->c -= mA * impulse;
        constraint.bodyPosA->p -= mA * impulse;
        constraint.bodyPosA->a -= math::Radian<double>{iA * twoDVectorCross(rA, impulse)};

        constraint.bodyPosB->c += mB * impulse;
        constraint.bodyPosB->p += mB * impulse;
        constraint.bodyPosB->a += math::Radian<double>{iB * twoDVectorCross(rB, impulse)};

        return positionError <= physic::linearSlop;
}

static inline void initializePivotJointConstraint(PivotJointConstraint & constraint, const aunteater::Timer & aTimer)
{
        constraint.bodyPosA = constraint.cbA->bodyPos;
        constraint.bodyPosB = constraint.cbB->bodyPos;
        constraint.velocityA = constraint.cbA->velocity;
        constraint.velocityB = constraint.cbB->velocity;
        constraint.rA = transformVector(constraint.localAnchorA - (constraint.bodyPosA->c - constraint.bodyPosA->p).as<math::Position>(), constraint.bodyPosA->a);
        constraint.rB = transformVector(constraint.localAnchorB - (constraint.bodyPosB->c - constraint.bodyPosB->p).as<math::Position>(), constraint.bodyPosB->a);
        constraint.angVecA = {-constraint.rA.y(), constraint.rA.x()};
        constraint.angVecB = {-constraint.rB.y(), constraint.rB.x()};

        Vec2 rA = constraint.rA;
        Vec2 rB = constraint.rB;
        double mA = constraint.invMassA;
        double iA = constraint.invMoiA;
        double mB = constraint.invMassB;
        double iB = constraint.invMoiB;

        constraint.k.at(0,0) = mA + mB + rA.y() * rA.y() * iA + rB.y() * rB.y() * iB;
        constraint.k.at(0,1) = -rA.y() * rA.x() * iA - rB.y() * rB.x() * iB;
        constraint.k.at(1,0) = constraint.k.at(0,1);
        constraint.k.at(1,1) = mA + mB + rA.x() * rA.x() * iA + rB.x() * rB.x() * iB;

        //warm start constraint
        constraint.velocityA->v -= constraint.impulse * constraint.invMassA;
        constraint.velocityA->w -= constraint.invMoiA * twoDVectorCross(constraint.rA, constraint.impulse);
        constraint.velocityB->v += constraint.impulse * constraint.invMassB;
        constraint.velocityB->w += constraint.invMoiB * twoDVectorCross(constraint.rB, constraint.impulse);
}

Physics::Physics(aunteater::EntityManager & aEntityManager) :
    bodyObserver{this},
    pivotObserver{this}
{
    aEntityManager.getFamily<PhysicalBody>().registerObserver(&bodyObserver);
    aEntityManager.getFamily<Pivotable>().registerObserver(&pivotObserver);

    //Initiliazing the query functions
    queryFunctions[ShapeType_Hull][ShapeType_Hull] = QueryFacePenetration;
}

void Physics::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    velocities.clear();
    positions.clear();
    collisionBoxes.clear();
    //Updating all the positions of the proxy bodies
    for (auto & body : constructedBodies)
    {
        collisionBoxes.emplace_back(createTransformedCollisionBox(body.bodyRef, body.posRef));
        velocities.emplace_back(body.aasRef.speed, body.aasRef.w);
        positions.emplace_back(body.posRef.position, static_cast<Position2>(body.bodyRef.massCenter.as<math::Vec>() + body.posRef.position.as<math::Vec>()), body.bodyRef.theta);
    }

    int i = 0;
    for (auto & body :constructedBodies)
    {
        body.synchronize(velocities, positions, collisionBoxes, i++);
        //body.debugRender();
    }
    // Broad phase
    // This should be done with a tree of hierarchical aabb for fastest aabb queries
    for (auto bodyAIt = constructedBodies.begin(); bodyAIt != --constructedBodies.end(); ++bodyAIt)
    {
        auto & bodyA = *bodyAIt;

        auto bodyBIt = bodyAIt;
        bodyBIt++;

        for (; bodyBIt != constructedBodies.end(); ++bodyBIt)
        {
            auto & bodyB = *bodyBIt;

            if (!bodyA.shouldCollide(bodyB))
            {
                continue;
            }

            auto aabbA = bodyA.box->shape.getAABB();
            auto aabbB = bodyB.box->shape.getAABB();

            /*
            debugDrawer->drawOutline({
                    aabbA.mPosition,
                    aabbA.mDimension,
                    math::Matrix<3, 3>::Identity(),
                    {210,100,255}
            });

            debugDrawer->drawOutline({
                    aabbB.mPosition,
                    aabbB.mDimension,
                    math::Matrix<3, 3>::Identity(),
                    {210,100,255}
            });
            */
            //Basic SAT for AABB
            if (aabbA.x() <= aabbB.x() + aabbB.width() &&
               aabbA.x() + aabbA.width() >= aabbB.x() &&
               aabbA.y() <= aabbB.y() + aabbB.height() &&
               aabbA.y() + aabbA.height() >= aabbB.y() &&
               (bodyA.bodyType != BodyType_Static || bodyB.bodyType != BodyType_Static)
               )
            {
                addPair(bodyA, bodyB, collidingBodies);
            }
        }
    }

    //Collide pairs
    auto pairIt = collidingBodies.begin();

    while (pairIt != collidingBodies.end())
    {
        auto & collisionPair = *pairIt;

        if (collisionPair.toRemove)
        {
            //Destroy pair here
            pairIt = collidingBodies.erase(pairIt);
            continue;
        }

        auto & bodyA = collisionPair.bodyA;
        auto & bodyB = collisionPair.bodyB;
        const auto oldManifold = collisionPair.manifold;

        ContactManifold & manifold = collisionPair.manifold;
        bool isTouching = getBestQuery(manifold, bodyA, bodyB);

        if (isTouching)
        {
            //This is definitely not efficient and there is probably a solution
            //to avoid the branch
            //Those needs to be pointer
            ConstructedBody * bodyRef = &bodyB;
            ConstructedBody * bodyInc = &bodyA;

            if (manifold.face == ContactManifold::ReferenceFace::FACEA)
            {
                bodyRef = &bodyA;
                bodyInc = &bodyB;
            }

            Shape::Edge refEdge = bodyRef->box->shape.getEdge(manifold.referenceEdgeIndex);

            manifold.incidentEdgeIndex = findIncidentEdge(refEdge.direction, bodyInc->box->shape);

            manifold.contacts = getContactPoints(
                manifold.referenceEdgeIndex,
                manifold.incidentEdgeIndex,
                bodyRef->box->shape,
                bodyInc->box->shape
            );

            //Now we check for persisted contact equivalence
            for (auto & oldContact : oldManifold.contacts)
            {
                for (auto & newContact : manifold.contacts)
                {
                    if (
                            newContact.indexIncident == oldContact.indexIncident &&
                            newContact.indexReference == oldContact.indexReference &&
                            newContact.typeIncident == oldContact.typeIncident &&
                            newContact.typeReference == oldContact.typeReference
                       )
                    {
                        newContact.normalImpulse = oldContact.normalImpulse;
                        newContact.tangentImpulse = oldContact.tangentImpulse;
                    }
                }
            }


            collisionPair.manifold = manifold;

            //Now we create the different contact constraints
            //We need to put everything back in the right order for this to work

            if (
                    (bodyRef->collisionType == CollisionType_Player &&
                    bodyInc->collisionType == CollisionType_Static_Env) || 
                    (bodyInc->collisionType == CollisionType_Player &&
                    bodyRef->collisionType == CollisionType_Static_Env)
               )
            {
                ConstructedBody * bodyPlayer = bodyRef->collisionType == CollisionType_Player ? bodyRef : bodyInc;
                Vec2 normal = bodyRef->collisionType == CollisionType_Player ? -manifold.normal : manifold.normal;
                playerConstraints.emplace_back(PlayerEnvironmentConstraint{
                        normal,
                        manifold.separation,
                        bodyPlayer
                        });
            }
            else if (
                    (bodyRef->collisionType == CollisionType_Moving_Env ||
                    bodyInc->collisionType == CollisionType_Moving_Env ) &&
                    !(bodyRef->collisionType == CollisionType_Player || bodyInc->collisionType == CollisionType_Player)
               )
            {
                for (auto & contact : manifold.contacts)
                {
                    Vec2 rA = contact.contactPoint.as<math::Vec>() - (bodyRef->bodyPos->c.as<math::Vec>());
                    Vec2 rB = contact.contactPoint.as<math::Vec>() - (bodyInc->bodyPos->c.as<math::Vec>());
                    Vec2 tangent = {manifold.normal.y(), -manifold.normal.x()};
                    velocityConstraints.emplace_back(VelocityConstraint{
                        bodyRef->velocity,
                        bodyRef->bodyPos,
                        rA,
                        twoDVectorCross(rA, manifold.normal),
                        twoDVectorCross(rA, tangent),
                        bodyRef->invMass,
                        bodyRef->invMoi,
                        0.,
                        math::Radian<double>(0.),
                        bodyInc->velocity,
                        bodyInc->bodyPos,
                        rB,
                        twoDVectorCross(rB, manifold.normal),
                        twoDVectorCross(rB, tangent),
                        bodyInc->invMass,
                        bodyInc->invMoi,
                        0.,
                        math::Radian<double>(0.),

                        sqrt(bodyRef->friction * bodyInc->friction),
                        std::max(bodyRef->friction, bodyInc->friction),
                        manifold.normal,
                        tangent,
                        manifold.localPoint,
                        
                        contact,
                    });
                }
            }
        }

        if (collisionPair.cold == true)
        {
            //Destroy pair here
            collisionPair.bodyA.contactList.erase(
                    collisionPair.iteratorA
                    );
            collisionPair.bodyB.contactList.erase(
                    collisionPair.iteratorB
                    );
            pairIt = collidingBodies.erase(pairIt);
        }
        else 
        {
            collisionPair.cold = true;
            ++pairIt;
        }
    }

    //Init joint constraint
    for (auto & pjConstraint : pivotJointConstraints)
    {
        initializePivotJointConstraint(pjConstraint, aTimer);
    }
    

    //Solve constraints
    for (VelocityConstraint & constraint : velocityConstraints)
    {
        // Warm starting the constraint
        ContactFeature & cf = constraint.cf;

        Vec2 impulseVec = cf.normalImpulse * constraint.normal + cf.tangentImpulse * constraint.tangent;

        constraint.velocityA->v += impulseVec * constraint.invMassA;
        constraint.velocityA->w += twoDVectorCross(constraint.rA, impulseVec) * constraint.invMoiA;
        constraint.velocityB->v -= impulseVec * constraint.invMassB;
        constraint.velocityB->w -= twoDVectorCross(constraint.rB, impulseVec) * constraint.invMassB;
    }

    for (int i = 0; i < maxVelocityConstraintIteration; i++)
    {
        for (PivotJointConstraint & pjConstraint : pivotJointConstraints)
        {
            solvePivotJointVelocityConstraint(pjConstraint, aTimer);
        }

        for (VelocityConstraint & constraint : velocityConstraints)
        {
            constraint.debugRender();
            solveContactVelocityConstraint(constraint, aTimer);
        }
    }

    for (VelocityConstraint & constraint : velocityConstraints)
    {
        constexpr double maxRotation = 0.5 * math::pi<double>;
        Vec2 v = constraint.velocityA->v;
        Vec2 translation = v * aTimer.delta();
        double w = constraint.velocityA->w;
        double rotation = constraint.velocityA->w * aTimer.delta();
        if (translation.dot(translation) > 4.)
        {
            v *= 2. / translation.getNorm();
        }
        if (rotation * rotation > maxRotation * maxRotation)
        {
            w *= maxRotation / std::abs(rotation);
        }

        constraint.bodyPosA->p += v * aTimer.delta();
        constraint.bodyPosA->c += v * aTimer.delta();
        constraint.bodyPosA->a += math::Radian<double>{w * aTimer.delta()};

        v = constraint.velocityB->v;
        translation = v * aTimer.delta();
        w = constraint.velocityB->w;
        rotation = constraint.velocityB->w * aTimer.delta();
        if (translation.dot(translation) > 4.)
        {
            v *= 2. / translation.getNorm();
        }
        if (rotation * rotation > maxRotation * maxRotation)
        {
            w *= maxRotation / std::abs(rotation);
        }
        constraint.bodyPosB->p += v * aTimer.delta();
        constraint.bodyPosB->c += v * aTimer.delta();
        constraint.bodyPosB->a += math::Radian<double>{w * aTimer.delta()};

        constraint.angleBaseA = constraint.bodyPosA->a;
        constraint.angleBaseB = constraint.bodyPosB->a;
    }

    for (int i = 0; i < maxPositionConstraintIteration; i++)
    {
        bool jointOkay = true;
        for (PivotJointConstraint & constraint : pivotJointConstraints)
        {
            jointOkay = solvePivotJointPositionConstraint(constraint) && jointOkay;
        }

        for (VelocityConstraint & constraint : velocityConstraints)
        {
            Vec2 rA = transformVector(constraint.rA, constraint.bodyPosA->a - constraint.angleBaseA);
            Vec2 rB = transformVector(constraint.rB, constraint.bodyPosB->a - constraint.angleBaseB);
            Vec2 normal = transformVector(-constraint.normal, constraint.bodyPosA->a - constraint.angleBaseA);
            Vec2 clipPoint = transformVector(constraint.clipPoint, constraint.bodyPosA->a - constraint.angleBaseA);
            Position2 onEdgePoint = constraint.bodyPosA->c + clipPoint;

            Position2 point = constraint.bodyPosB->c + rB;
            double separation = (point.as<math::Vec>() - onEdgePoint.as<math::Vec>()).dot(normal) - physic::linearSlop * 4.;

            double C = std::min(0., std::max(-.2, .2 * (separation + physic::linearSlop)));

            double rnA = twoDVectorCross(rA, normal);
            double rnB = twoDVectorCross(rB, normal);
            double totalMass = constraint.invMassA + constraint.invMassB + constraint.invMoiA * rnA * rnA + constraint.invMoiB * rnB * rnB;

            Vec2 impulse = totalMass > 0. ? (- C / totalMass) * normal : Vec2::Zero();

            constraint.bodyPosA->c -= constraint.invMassA * impulse;
            constraint.bodyPosA->p -= constraint.invMassA * impulse;
            constraint.bodyPosA->a -= math::Radian<double>{constraint.invMoiA * twoDVectorCross(rA, impulse)};

            constraint.bodyPosB->c += constraint.invMassB * impulse;
            constraint.bodyPosB->p += constraint.invMassB * impulse;
            constraint.bodyPosB->a += math::Radian<double>{constraint.invMoiB * twoDVectorCross(rB, impulse)};

            jointOkay = jointOkay && separation >= -3. * physic::linearSlop;
        }

        if (jointOkay)
        {
            break;
        }
    }

    for (auto & constraint : playerConstraints)
    {
        Position2 oldP = constraint.cPlayer->bodyPos->p;
        constraint.cPlayer->bodyPos->p += constraint.separation * constraint.normal;
        Vec2 tangent = {-constraint.normal.y(), constraint.normal.x()};
        constraint.cPlayer->velocity->v = constraint.cPlayer->velocity->v.dot(tangent) * tangent;

        if (-constraint.normal.dot(PlayerGroundedNormal) > PlayerGroundedSlopeDotValue)
        {
            //Set player as grounded
            constraint.cPlayer->velocity->v *= PlayerGroundFriction;
            constraint.cPlayer->entity->get<PlayerData>().state = PlayerCollisionState_Grounded;
        }
        else if (constraint.normal.dot(PlayerWalledNormal) > PlayerWallSlopeDotValue || constraint.normal.dot(PlayerWalledNormal) < -PlayerWallSlopeDotValue)
        {
            //Set player as walled
            constraint.cPlayer->entity->get<PlayerData>().state = PlayerCollisionState_Walled;
        }
    }

    //Update entities body
    for (auto & body : constructedBodies)
    {
        body.updateEntity();
        body.box->shape.debugRender();
    }

    //Cleaning up
    velocityConstraints.clear();
    playerConstraints.clear();
}

} // namespace grapito
} // namespace ad
