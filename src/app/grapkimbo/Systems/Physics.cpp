#include "Physics.h"
#include "Components/Body.h"
#include "math/Matrix.h"
#include <iostream>
#include <variant>

namespace ad {
namespace grapito
{

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

std::vector<ContactFeature> CheckContactValidity(const Shape::Edge & referenceEdge, std::array<ContactFeature, 2> points)
{
    std::vector<ContactFeature> result;

    double separation = std::numeric_limits<double>::max();

    separation = std::min(separation, referenceEdge.origin.as<math::Vec>().dot(referenceEdge.normal));
    separation = std::min(separation, referenceEdge.end.as<math::Vec>().dot(referenceEdge.normal));

    for (auto contact : points)
    {
        double candidateSeparation = contact.contactPoint.as<math::Vec>().dot(referenceEdge.normal);
        //std::cout << "projectPoint" << projectedPoint << "\n";

        //This is sad that we get fucked by smaller than difference 0.01mm
        if (
                candidateSeparation < separation + 0.00001
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
        //std::cout << "hasInterA" << "\n";
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
        //std::cout << "incident.end" << "\n";
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
        //std::cout << "hasInterB" << "\n";
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
        //std::cout << "incident.origin" << "\n";
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


Physics::Physics(aunteater::EntityManager & aEntityManager)
{
    aEntityManager.getFamily<PhysicalBody>().registerObserver(this);

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
    for (auto pairIt = collidingBodies.begin(); pairIt != collidingBodies.end(); ++pairIt)
    {
        auto & collisionPair = *pairIt;
        //collisionPair.debugRender();
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
            //std::cout << manifold.contacts.size() << "\n";

            //Now we create the different contact constraints
            //We need to put everything back in the right order for this to work
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
                        bodyInc->velocity,
                        bodyInc->bodyPos,
                        rB,
                        twoDVectorCross(rB, manifold.normal),
                        twoDVectorCross(rB, tangent),
                        bodyInc->invMass,
                        bodyInc->invMoi,
                        0.,

                        sqrt(bodyRef->friction * bodyInc->friction),
                        std::max(bodyRef->friction, bodyInc->friction),
                        manifold.normal,
                        tangent,
                        
                        contact,
                        bodyRef,
                        bodyInc
                });
            }
        }

        if (collisionPair.cold == true)
        {
            /*std::cout << collisionPair;
            //Destroy pair here
            collisionPair.bodyA.contactList.erase(
                    collisionPair.iteratorA
                    );
            collisionPair.bodyB.contactList.erase(
                    collisionPair.iteratorB
                    );
            collidingBodies.erase(pairIt++);
            */
        }

        collisionPair.cold = true;
    }
    

    //std::cout << velocityConstraints.size() << "\n";
    //Solve constraints
    for (VelocityConstraint & constraint : velocityConstraints)
    {
        // Warm starting the constraint
        // TODO warm starting is wrong it should be multiplied by delta
        // and it should just be preload the contact impulse with the impulse not
        // apply it to the bodies
        ContactFeature & cf = constraint.cf;

        Vec2 tangent = {constraint.normal.y(), -constraint.normal.x()};

        Vec2 impulseVec = cf.normalImpulse * constraint.normal + cf.tangentImpulse * tangent;

        applyImpulse(
                impulseVec * constraint.invMassA,
                twoDVectorCross(constraint.rA, impulseVec) * constraint.invMoiA,
                -impulseVec * constraint.invMassB,
                -twoDVectorCross(constraint.rB, impulseVec) * constraint.invMassB,
                constraint,
                aTimer.delta()
                );
    }

    for (int i = 0; i < maxNormalConstraintIteration; i++)
    {
        for (VelocityConstraint & constraint : velocityConstraints)
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


                Vec2 impVecA = lambda * constraint.tangent * constraint.invMassA;
                double angularImpulseA = lambda * constraint.crossATangent * constraint.invMoiA;


                Vec2 impVecB = -lambda * constraint.tangent * constraint.invMassB;
                double angularImpulseB = -lambda * constraint.crossBTangent * constraint.invMoiB;

                //std::cout << "pre tangent impulse";
                //std::cout << constraint;
                applyImpulse(
                        impVecA,
                        angularImpulseA,
                        impVecB,
                        angularImpulseB,
                        constraint,
                        aTimer.delta()
                        );
                //std::cout << "post tangent impulse";
                //std::cout << constraint;
            }

            Vec2 speed = constraint.velocityA->v + (AngVecA * constraint.velocityA->w) - constraint.velocityB->v - (AngVecB * constraint.velocityB->w);
            double normalSpeed = speed.dot(constraint.normal);

            double lambda = -(1 / (totalMass + totalAngularMass)) * (normalSpeed);
            double newImpulse = std::max(cf.normalImpulse + lambda, 0.);
            lambda = newImpulse - cf.normalImpulse;
            cf.normalImpulse = newImpulse;

            Vec2 impulseVecA = lambda * constraint.normal * constraint.invMassA;
            double angularImpulseA = lambda * constraint.crossA * constraint.invMoiA;

            Vec2 impulseVecB = -lambda * constraint.normal * constraint.invMassB;
            double angularImpulseB = -lambda * constraint.crossB * constraint.invMoiB;

            //std::cout << "pre normal impulse";
            //std::cout << constraint;
            applyImpulse(
                    impulseVecA,
                    angularImpulseA,
                    impulseVecB,
                    angularImpulseB,
                    constraint,
                    aTimer.delta()
                    );
            //std::cout << "pre normal impulse";
            //std::cout << constraint;
            constraint.debugRender();
        }
    }

    //Update entities body
    for (auto & body : constructedBodies)
    {
        //std::cout << body;
        body.updateEntity(aTimer.delta());
        body.box->shape.debugRender();
    }

    //Cleaning up
    velocityConstraints.clear();
}

} // namespace grapito
} // namespace ad
