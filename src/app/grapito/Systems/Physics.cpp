#include "Physics.h"

#include "../Configuration.h"
#include "../Timer.h"

#include "../Components/Body.h"
#include "../Components/PivotJoint.h"
#include "../Components/PlayerData.h"
#include "../Components/WeldJoint.h"

#include "../Utils/HomogeneousTransformation.h"
#include "../Utils/PhysicsStructs.h"
#include "Entities.h"
#include "math/Color.h"

#include <math/Angle.h>
#include <math/Interpolation/Interpolation.h>
#include <math/Matrix.h>

#include <spdlog/spdlog.h>

#include <iostream>
#include <ostream>

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
        if (&body == &collisionPair->bodyA)
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

    for (aunteater::weak_entity entity : body.jointEntityList)
    {
        entity->markToRemove();
    }

    mPhysicsSystem->constructedBodies.erase(aEntity.get<Body>().constructedBodyIt);
    aEntity.get<Body>().constructedBodyIt = mPhysicsSystem->constructedBodies.end();
}

template<class T_joint, class T_joint_constraint>
JointObserver<T_joint, T_joint_constraint>::JointObserver(Physics * aPhysicsSystem) :
    mPhysicsSystem{aPhysicsSystem}
{}

template<class T_joint, class T_joint_constraint>
void JointObserver<T_joint, T_joint_constraint>::addedEntity(aunteater::LiveEntity & aEntity)
{
    //assert(!aEntity.get<PivotJoint>().bodyA->has<Body>() && "Pivot jointed entity A must have a body component");
    //assert(!aEntity.get<PivotJoint>().bodyB->has<Body>() && "Pivot jointed entity B must have a body component");
    T_joint & joint = aEntity.get<T_joint>();
    ConstructedBody & bodyA = *joint.bodyA->template get<Body>().constructedBodyIt;
    ConstructedBody & bodyB = *joint.bodyB->template get<Body>().constructedBodyIt;

    mPhysicsSystem->jointConstraints.push_back(
        JointVariant(
            joint,

            &bodyA,
            &bodyB,

            &aEntity
        )
    );

    size_t jointIndex = mPhysicsSystem->jointConstraints.size() - 1;

    auto itA = bodyA.jointEntityList.insert(
            bodyA.jointEntityList.end(),
            &aEntity
    );
    auto itB = bodyB.jointEntityList.insert(
            bodyB.jointEntityList.end(),
            &aEntity
    );

    aEntity.get<T_joint>().constraintIndex = jointIndex;
    aEntity.get<T_joint>().constructedBodyEntityItB = itB;
    aEntity.get<T_joint>().constructedBodyEntityItA = itA;
}

template<class T_joint, class T_joint_constraint>
void JointObserver<T_joint, T_joint_constraint>::removedEntity(aunteater::LiveEntity & aEntity)
{
    T_joint & joint = aEntity.get<T_joint>();

    if (aEntity.get<T_joint>().bodyA->template has<Body>() &&
        aEntity.get<T_joint>().bodyA->template get<Body>().constructedBodyIt != mPhysicsSystem->constructedBodies.end()
       )
    {
        ConstructedBody & bodyA = *aEntity.get<T_joint>().bodyA->template get<Body>().constructedBodyIt;
        bodyA.jointEntityList.erase(joint.constructedBodyEntityItA);
    }
    if (aEntity.get<T_joint>().bodyB->template has<Body>() &&
        aEntity.get<T_joint>().bodyB->template get<Body>().constructedBodyIt != mPhysicsSystem->constructedBodies.end()
       )
    {
        ConstructedBody & bodyB = *aEntity.get<T_joint>().bodyB->template get<Body>().constructedBodyIt;
        bodyB.jointEntityList.erase(joint.constructedBodyEntityItB);
    }

    //We can't switch joint constraint from the back if there is only one constraint in the vector
    //Or if we removing the last constraint
    if (mPhysicsSystem->jointConstraints.size() - 1 != joint.constraintIndex)
    {
        //We take the last constraint of the vector
        JointVariant lastConstraint = mPhysicsSystem->jointConstraints.back();
        //And remove it from the end of the vector of constraint
        mPhysicsSystem->jointConstraints.pop_back();
        //We then put it in place of the removed constraint
        mPhysicsSystem->jointConstraints.at(joint.constraintIndex) = lastConstraint;
        //And adjust the index of the moved constraint in the corresponding entity
        switch(lastConstraint.mType)
        {
            case JointType_Distance:
            {
                auto constraint = std::get<DistanceJointConstraint>(lastConstraint.mConstraint);
                constraint.jointEntity->get<DistanceJoint>().constraintIndex = joint.constraintIndex;
                break;
            }
            case JointType_Pivot:
            {
                auto constraint = std::get<PivotJointConstraint>(lastConstraint.mConstraint);
                constraint.jointEntity->get<PivotJoint>().constraintIndex = joint.constraintIndex;
                break;
            }
            case JointType_Weld:
            {
                auto constraint = std::get<WeldJointConstraint>(lastConstraint.mConstraint);
                constraint.jointEntity->get<WeldJoint>().constraintIndex = joint.constraintIndex;
                break;
            }
        }
    }
    else
    {
        mPhysicsSystem->jointConstraints.erase(mPhysicsSystem->jointConstraints.begin() + joint.constraintIndex);
    }
}


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

static inline double distanceToLine(Position2 point, Position2 origin, Position2 end)
{
    return (((end.x() - origin.x()) * (origin.y() - point.y())) - ((origin.x() - point.x()) * (end.y() - origin.y()))) / (origin - end).getNorm();
}

static inline ContactManifold QueryFacePenetration(
    const ConstructedBody & bodyA,
    const ConstructedBody & bodyB
)
{
    ContactManifold resultManifold;

    for (size_t i = 0; i < bodyA.box->shape.mFaceCount; ++i)
    {
        const Shape::Edge edgeA = bodyA.box->shape.getEdge(i);
        const Position2 support = bodyB.box->getSupport(-edgeA.normal);
        const double distance = distanceToLine(support, edgeA.origin, edgeA.end);


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


static inline bool getBestQuery(
    ContactManifold & resultManifold,
    const ConstructedBody & bodyA,
    const ConstructedBody & bodyB
)
{
    ContactManifold queryA = queryFunctions[bodyA.shapeType][bodyB.shapeType](bodyA, bodyB);


    if (queryA.separation > physic::gLinearSlop)
    {
        return false;
    }

    ContactManifold queryB = queryFunctions[bodyB.shapeType][bodyA.shapeType](bodyB, bodyA);

    if (queryB.separation > physic::gLinearSlop)
    {
        return false;
    }
    
    // We want the highest distance to minimize displacement because those distance are
    // negative !
    if (queryB.separation > queryA.separation)
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
    float bestDot = std::numeric_limits<float>::max();
    int incidentIndex;

    // We want to find the most antiparallel edge to our referenceEdge
    // The edgeDirection direction is reversed according to the referenceEdge
    // (this is to get a correctly oriented reference frame)
    // so we compute the dot of all incident edges with the edgeDirection 
    // and we store the maximum
    for (size_t i = 0; i < incidentShape.mFaceCount; ++i)
    {
        Shape::Edge edge = incidentShape.getEdge(i);
        
        float dot = edgeDirection.dot(edge.end - edge.origin);

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

    float separation = std::numeric_limits<float>::max();

    separation = std::min(separation, referenceEdge.origin.as<math::Vec>().dot(referenceEdge.normal));
    separation = std::min(separation, referenceEdge.end.as<math::Vec>().dot(referenceEdge.normal));

    for (auto contact : points)
    {
        float candidateSeparation = contact.contactPoint.as<math::Vec>().dot(referenceEdge.normal);

        //This is sad that we get fucked by smaller than difference 0.01mm
        if (
                candidateSeparation < separation + physic::gLinearSlop
           )
        {
            result.emplace_back(contact);
        }
    }

    return result;
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
    Position2 intersectA{0.f, 0.f};
    Position2 intersectB{0.f, 0.f};
    Vec2 edgeVector = incidentEdge.end - incidentEdge.origin;
    Vec2 edgeNormal = {edgeVector.y(), -edgeVector.x()};
    Line incidentLine{incidentEdge.origin + edgeNormal * physic::gLinearSlop, incidentEdge.end - incidentEdge.origin + edgeNormal * physic::gLinearSlop};

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
                vertex + aPos.position.as<math::Vec>(),
                aBody.theta,
                aBody.massCenter + aPos.position.as<math::Vec>()
                );
        transformedVertices.emplace_back(transformedPos);
    }

    return transformedVertices;
}

static inline void solveContactVelocityConstraint(VelocityConstraint & constraint)
{

    ContactFeature & cf = constraint.cf;
    Vec2 AngVecA = constraint.angVecA;
    Vec2 AngVecB = constraint.angVecB;

    float totalMass = constraint.totalMass;

    float totalAngularMass = constraint.totalNormalAngularMass;

    if (constraint.friction > 0.)
    {
        Vec2 speed = constraint.velocityA->v + (AngVecA * constraint.velocityA->w) - constraint.velocityB->v - (AngVecB * constraint.velocityB->w);

        float tangentSpeed = speed.dot(constraint.tangent);

        float totalAngularTangentMass = constraint.totalTangentAngularMass;

        float lambda = -(1 / (totalMass + totalAngularTangentMass)) * (tangentSpeed);
        float maxFriction = constraint.friction * cf.normalImpulse;
        float newImpulseTangent = std::max(std::min(cf.tangentImpulse + lambda, maxFriction), -maxFriction);
        lambda = newImpulseTangent - cf.tangentImpulse;
        cf.tangentImpulse = newImpulseTangent;

        Vec2 impulse = lambda * constraint.tangent;
        float angularImpulseA = lambda * constraint.crossATangent * constraint.invMoiA;
        float angularImpulseB = lambda * constraint.crossBTangent * constraint.invMoiB;

        constraint.velocityA->v += impulse * constraint.invMassA;
        constraint.velocityA->w += angularImpulseA;
        constraint.velocityB->v -= impulse * constraint.invMassB;
        constraint.velocityB->w -= angularImpulseB;
    }

    Vec2 speed = constraint.velocityA->v + (AngVecA * constraint.velocityA->w) - constraint.velocityB->v - (AngVecB * constraint.velocityB->w);
    float normalSpeed = speed.dot(constraint.normal);

    float lambda = -(1 / (totalMass + totalAngularMass)) * (normalSpeed);
    float newImpulse = std::max(cf.normalImpulse + lambda, 0.f);
    lambda = newImpulse - cf.normalImpulse;
    cf.normalImpulse = newImpulse;

    Vec2 impulse = lambda * constraint.normal;
    float angularImpulseA = lambda * constraint.crossA * constraint.invMoiA;

    float angularImpulseB = lambda * constraint.crossB * constraint.invMoiB;

    constraint.velocityA->v += impulse * constraint.invMassA;
    constraint.velocityA->w += angularImpulseA;
    constraint.velocityB->v -= impulse * constraint.invMassB;
    constraint.velocityB->w -= angularImpulseB;
}

Physics::Physics(aunteater::EntityManager & aEntityManager) :
    bodyObserver{this},
    pivotObserver{this},
    weldObserver{this},
    distanceObserver{this},
    mEntityManager{aEntityManager}
{
    aEntityManager.getFamily<PhysicalBody>().registerObserver(&bodyObserver);
    aEntityManager.getFamily<Pivotable>().registerObserver(&pivotObserver);
    aEntityManager.getFamily<Weldable>().registerObserver(&weldObserver);
    aEntityManager.getFamily<Distanceable>().registerObserver(&distanceObserver);

    //Initiliazing the query functions
    queryFunctions[ShapeType_Hull][ShapeType_Hull] = QueryFacePenetration;

    //We just reserve an arbitrary (2^n) value of velocities, positions, collisionBoxes
    //This is just to avoid the vector going through the basic vector growth
    //which is 1, 2, 4, 8, 16...
    velocities.reserve(128);
    positions.reserve(128);
    collisionBoxes.reserve(128);
}

void Physics::update(const GrapitoTimer aTimer, const GameInputState &)
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
    }

    // Broad phase
    // This should be done with a tree of hierarchical aabb for fastest aabb queries
    for (auto bodyAIt = constructedBodies.begin(); bodyAIt != --constructedBodies.end(); ++bodyAIt)
    {
        auto & bodyA = *bodyAIt;

        auto bodyBIt = bodyAIt;
        bodyBIt++;

#if defined(SHRED_DEBUG_RENDER)
        bodyA.box->shape.debugRender();
#endif
        auto aabbA = bodyA.box->shape.getAABB();

        for (; bodyBIt != constructedBodies.end(); ++bodyBIt)
        {
            auto & bodyB = *bodyBIt;

            if (!bodyA.shouldCollide(bodyB) && !bodyA.shouldDetect(bodyB))
            {
                continue;
            }

            auto aabbB = bodyB.box->shape.getAABB();

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

            if (manifold.incidentEdgeIndex == -1)
            {
                std::stringstream bodyRefString;
                bodyRefString << *bodyRef;
                std::stringstream bodyIncString;
                bodyIncString << *bodyInc;
                spdlog::get("grapito")->error("IncidentEdge not found");
                spdlog::get("grapito")->error("bodyRef: {} ", bodyRefString.str());
                spdlog::get("grapito")->error("bodyInc: {} ", bodyIncString.str());
                mEntityManager.addEntity(makeHudText("STOP#### There was a incidentEdge crash Please look at the log!!!!!!!!!!", {-300.f, 300.f}, ScreenPosition::BottomLeft));
                continue;
            }

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
                        //We know the contact are the same so we copy the previous
                        //frame impulse
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
                    bodyRef->shouldCollide(*bodyInc) && 
                    (bodyRef->collisionType == CollisionType_Moving_Env ||
                    bodyInc->collisionType == CollisionType_Moving_Env ) &&
                    !(bodyRef->collisionType == CollisionType_Player || bodyInc->collisionType == CollisionType_Player)
               )
            {
                for (auto & contact : manifold.contacts)
                {
                    collisionPair.cold = false;
                    Vec2 tangent = {manifold.normal.y(), -manifold.normal.x()};

                    Vec2 rA = contact.contactPoint.as<math::Vec>() - (bodyRef->bodyPos->c.as<math::Vec>());
                    float crossA = twoDVectorCross(rA, manifold.normal);
                    float crossATangent = twoDVectorCross(rA, tangent);

                    Vec2 rB = contact.contactPoint.as<math::Vec>() - (bodyInc->bodyPos->c.as<math::Vec>());
                    float crossB = twoDVectorCross(rB, manifold.normal);
                    float crossBTangent = twoDVectorCross(rB, tangent);


                    velocityConstraints.emplace_back(VelocityConstraint{
                        bodyRef->velocity,
                        bodyRef->bodyPos,
                        rA,
                        {-rA.y(), rA.x()},
                        crossA,
                        crossA * crossA,
                        crossATangent,
                        crossATangent * crossATangent,
                        bodyRef->invMass,
                        bodyRef->invMoi,
                        math::Radian<float>{0.f},

                        bodyInc->velocity,
                        bodyInc->bodyPos,
                        rB,
                        {-rB.y(), rB.x()},
                        crossB,
                        crossB * crossB,
                        crossBTangent,
                        crossBTangent * crossBTangent,
                        bodyInc->invMass,
                        bodyInc->invMoi,
                        math::Radian<float>{0.f},

                        bodyRef->invMass + bodyInc->invMass,
                        bodyRef->invMoi * crossA * crossA + bodyInc->invMoi * crossB * crossB,
                        bodyRef->invMoi * crossATangent * crossATangent + bodyInc->invMoi * crossBTangent * crossBTangent,
                        sqrt(bodyRef->friction * bodyInc->friction),
                        std::max(bodyRef->friction, bodyInc->friction),
                        manifold.normal,
                        tangent,
                        manifold.localPoint,
                        
                        contact,
                    });
#if defined(SHRED_DEBUG_RENDER)
                    velocityConstraints.back().debugRender();
#endif
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

    float timerRatio = aTimer.delta() * mInvOldTimer;

    //Init joint constraint
    for (auto & constraintVariant : jointConstraints)
    {
        switch(constraintVariant.mType)
        {
            case JointType_Distance:
            {
                auto & constraint = std::get<DistanceJointConstraint>(constraintVariant.mConstraint);
                constraint.InitVelocityConstraint(aTimer, timerRatio);
                break;
            }
            [[likely]]case JointType_Pivot:
            {
                auto & constraint = std::get<PivotJointConstraint>(constraintVariant.mConstraint);
                constraint.InitVelocityConstraint(aTimer, timerRatio);
                break;
            }
            case JointType_Weld:
            {
                auto & constraint = std::get<WeldJointConstraint>(constraintVariant.mConstraint);
                constraint.InitVelocityConstraint(aTimer, timerRatio);
                break;
            }
        }
    }
    

    //Solve constraints
    for (VelocityConstraint & constraint : velocityConstraints)
    {
        // Warm starting the constraint
        ContactFeature & cf = constraint.cf;

        Vec2 impulseVec = cf.normalImpulse * constraint.normal + cf.tangentImpulse * constraint.tangent * timerRatio;

        constraint.velocityA->v += impulseVec * constraint.invMassA;
        constraint.velocityA->w += twoDVectorCross(constraint.rA, impulseVec) * constraint.invMoiA;
        constraint.velocityB->v -= impulseVec * constraint.invMassB;
        constraint.velocityB->w -= twoDVectorCross(constraint.rB, impulseVec) * constraint.invMassB;
    }

    for (int i = 0; i < physic::gMaxVelocityConstraintIteration; i++)
    {
        for (auto & constraintVariant : jointConstraints)
        {
            switch(constraintVariant.mType)
            {
                case JointType_Distance:
                {
                    auto & constraint = std::get<DistanceJointConstraint>(constraintVariant.mConstraint);
                    constraint.SolveVelocityConstraint(aTimer);
                    break;
                }
                [[likely]]case JointType_Pivot:
                {
                    auto & constraint = std::get<PivotJointConstraint>(constraintVariant.mConstraint);
                    constraint.SolveVelocityConstraint(aTimer);
                    break;
                }
                case JointType_Weld:
                {
                    auto & constraint = std::get<WeldJointConstraint>(constraintVariant.mConstraint);
                    constraint.SolveVelocityConstraint(aTimer);
                    break;
                }
            }
        }

        for (VelocityConstraint & constraint : velocityConstraints)
        {
            solveContactVelocityConstraint(constraint);
        }
    }

    for (auto & constraint : playerConstraints)
    {
        Vec2 tangent = {-constraint.normal.y(), constraint.normal.x()};
        float normalComponent = std::max(0.f, constraint.cPlayer->velocity->v.dot(-constraint.normal));
        Vec2 normalSpeed = normalComponent * -constraint.normal;

        //Since velocity integration is delayed after the physics system
        //we need to check that the y velocity is greater than 0
        //This indicates that the player is moving upwards and cannot be grounded for this frame
        if (
                -constraint.normal.dot(PlayerGroundedNormal) > PlayerGroundedSlopeDotValue
                && normalComponent <= 0.f
            )
        {
            if (
                !(constraint.cPlayer->entity->get<PlayerData>().state & PlayerCollisionState_Grounded)
               )
            {
                constraint.cPlayer->bodyPos->p += constraint.separation * constraint.normal;
                constraint.cPlayer->velocity->v = constraint.cPlayer->velocity->v.dot(tangent) * tangent + normalSpeed;
            }

            //Set player as grounded
            constraint.cPlayer->entity->get<PlayerData>().state |= PlayerCollisionState_Grounded;
            constraint.cPlayer->entity->get<PlayerData>().state &= ~PlayerCollisionState_Jumping;
        }
        else if ((constraint.normal.dot(PlayerWalledNormal) < -PlayerWallSlopeDotValue ||
            constraint.normal.dot(PlayerWalledNormal) > PlayerWallSlopeDotValue)
            && normalComponent <= 0.f
            )
        {
            if (
                !(constraint.cPlayer->entity->get<PlayerData>().state & PlayerCollisionState_Walled)
               )
            {
                constraint.cPlayer->bodyPos->p += constraint.separation * constraint.normal;
                constraint.cPlayer->velocity->v = constraint.cPlayer->velocity->v.dot(tangent) * tangent + normalSpeed;
            }

            constraint.cPlayer->entity->get<PlayerData>().state |= PlayerCollisionState_Walled;
            if (constraint.normal.x() > 0.f)
            {
                constraint.cPlayer->entity->get<PlayerData>().state |= PlayerCollisionState_WalledRight;
            }
            else
            {
                constraint.cPlayer->entity->get<PlayerData>().state |= PlayerCollisionState_WalledLeft;
            }
        }
        else
        {
            constraint.cPlayer->bodyPos->p += constraint.separation * constraint.normal;
            constraint.cPlayer->velocity->v = constraint.cPlayer->velocity->v.dot(tangent) * tangent + normalSpeed;

            constraint.cPlayer->entity->get<PlayerData>().wallClingFrameCounter = 0;
        }
#if defined(SHRED_DEBUG_RENDER)
        constraint.cPlayer->box->shape.debugRender();
#endif
    }

    //Position integration
    for (size_t i = 0; i < velocities.size(); i++)
    {
        auto & velocity = velocities[i];
        auto & position = positions[i];
        constexpr float maxRotation = 0.5 * math::pi<float>;
        Vec2 v = velocity.v;
        Vec2 translation = v * aTimer.delta();
        float w = velocity.w;
        float rotation = velocity.w * aTimer.delta();
        if (translation.dot(translation) > 4.)
        {
            v *= 2. / translation.getNorm();
        }
        if (rotation * rotation > maxRotation * maxRotation)
        {
            w *= maxRotation / std::abs(rotation);
        }

        position.p += v * aTimer.delta();
        position.c += v * aTimer.delta();
        position.a += math::Radian<float>{w * aTimer.delta()};
        velocity.v = v;
        velocity.w = w;
    }

    for (VelocityConstraint & constraint : velocityConstraints)
    {
        constraint.angleBaseA = constraint.bodyPosA->a;
        constraint.angleBaseB = constraint.bodyPosB->a;
    }
    for (int i = 0; i < physic::gMaxPositionConstraintIteration; i++)
    {
        bool jointOkay = true;
        for (auto & constraintVariant : jointConstraints)
        {
            switch(constraintVariant.mType)
            {
                case JointType_Distance:
                {
                    auto & constraint = std::get<DistanceJointConstraint>(constraintVariant.mConstraint);
                    jointOkay = constraint.SolvePositionConstraint() && jointOkay;
                    break;
                }
                [[likely]]case JointType_Pivot:
                {
                    auto & constraint = std::get<PivotJointConstraint>(constraintVariant.mConstraint);
                    jointOkay = constraint.SolvePositionConstraint() && jointOkay;
                    break;
                }
                case JointType_Weld:
                {
                    auto & constraint = std::get<WeldJointConstraint>(constraintVariant.mConstraint);
                    jointOkay = constraint.SolvePositionConstraint() && jointOkay;
                    break;
                }
            }
        }

        for (VelocityConstraint & constraint : velocityConstraints)
        {
            Vec2 rA = transformVector(constraint.rA, constraint.bodyPosA->a - constraint.angleBaseA);
            Vec2 rB = transformVector(constraint.rB, constraint.bodyPosB->a - constraint.angleBaseB);
            Vec2 normal = transformVector(-constraint.normal, constraint.bodyPosA->a - constraint.angleBaseA);
            Vec2 clipPoint = transformVector(constraint.clipPoint, constraint.bodyPosA->a - constraint.angleBaseA);
            Position2 onEdgePoint = constraint.bodyPosA->c + clipPoint;

            Position2 point = constraint.bodyPosB->c + rB;
            float separation = (point.as<math::Vec>() - onEdgePoint.as<math::Vec>()).dot(normal) - 4.f * physic::gLinearSlop;

            float C = std::min(0.f, std::max(-physic::gMaxLinearCorrection, physic::gBaumgarteFactor * (separation + physic::gLinearSlop)));

            float rnA = twoDVectorCross(rA, normal);
            float rnB = twoDVectorCross(rB, normal);
            float totalMass = constraint.invMassA + constraint.invMassB + constraint.invMoiA * rnA * rnA + constraint.invMoiB * rnB * rnB;

            Vec2 impulse = totalMass > 0. ? (- C / totalMass) * normal : Vec2::Zero();

            constraint.bodyPosA->c -= constraint.invMassA * impulse;
            constraint.bodyPosA->p -= constraint.invMassA * impulse;
            constraint.bodyPosA->a -= math::Radian<float>{constraint.invMoiA * twoDVectorCross(rA, impulse)};

            constraint.bodyPosB->c += constraint.invMassB * impulse;
            constraint.bodyPosB->p += constraint.invMassB * impulse;
            constraint.bodyPosB->a += math::Radian<float>{constraint.invMoiB * twoDVectorCross(rB, impulse)};

            jointOkay = jointOkay && separation >= -3. * physic::gLinearSlop;
        }

        if (jointOkay)
        {
            break;
        }
    }

    //Update entities body
    for (auto & body : constructedBodies)
    {
        body.updateEntity();
    }

    //Cleaning up
    velocityConstraints.clear();
    playerConstraints.clear();

    mInvOldTimer = 1 / aTimer.delta();
}

} // namespace grapito
} // namespace ad
