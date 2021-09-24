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

std::vector<ContactFeature> CheckContactValidity(const Shape::Edge & referenceEdge, const Vec2 & edgeDirection, std::array<ContactFeature, 2> points)
{
    std::vector<ContactFeature> result;

    // We use this algorithm to find all vertices with bit value (1|0)000
    // https://www.csd.uwo.ca/~sbeauche/CS3388/CS3388-2D-Clipping.pdf
    // Our y values are reversed because we use bottom right origin
    // This tests gives us all point beyond our colliding axis
    Position2 projectedReferenceOrigin = {
        (referenceEdge.normal.y() * referenceEdge.origin.x() - referenceEdge.normal.x() * referenceEdge.origin.y()) /
            (referenceEdge.normal.y() * edgeDirection.x() - referenceEdge.normal.x() * edgeDirection.y()),
        (edgeDirection.y() * referenceEdge.origin.x() - edgeDirection.x() * referenceEdge.origin.y()) /
            (referenceEdge.normal.x() * edgeDirection.y() - referenceEdge.normal.y() * edgeDirection.x())
    };
    Position2 projectedReferenceEnd = {
        (referenceEdge.normal.y() * referenceEdge.end.x() - referenceEdge.normal.x() * referenceEdge.end.y()) /
            (referenceEdge.normal.y() * edgeDirection.x() - referenceEdge.normal.x() * edgeDirection.y()),
        (edgeDirection.y() * referenceEdge.end.x() - edgeDirection.x() * referenceEdge.end.y()) /
            (referenceEdge.normal.x() * edgeDirection.y() - referenceEdge.normal.y() * edgeDirection.x())
    };

    for (auto contact : points)
    {
        Position2 projectedPoint = {
            (referenceEdge.normal.y() * contact.contactPoint.x() - referenceEdge.normal.x() * contact.contactPoint.y()) /
                (referenceEdge.normal.y() * edgeDirection.x() - referenceEdge.normal.x() * edgeDirection.y()),
            (edgeDirection.y() * contact.contactPoint.x() - edgeDirection.x() * contact.contactPoint.y()) /
                (referenceEdge.normal.x() * edgeDirection.y() - referenceEdge.normal.y() * edgeDirection.x())
        };

        if (
                projectedPoint.y() <= projectedReferenceOrigin.y() &&
                projectedPoint.x() <= projectedReferenceOrigin.x() &&
                projectedPoint.x() >= projectedReferenceEnd.x()
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
            incidentEdgeIndex,
            intersectA
        };
        candidate[0] = (contactFeature);
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
            incidentEdgeIndex,
            incidentEdge.end,
        };

        candidate[0] = (contactFeature);
    }

    if (hasInterB)
    {
        auto contactFeature = ContactFeature{
            ContactFeature::vertex,
            ContactFeature::face,
            static_cast<uint8_t>((referenceEdgeIndex + 1) % refShape.mFaceCount),
            incidentEdgeIndex,
            intersectA
        };

        candidate[1] = (contactFeature);
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
            referenceEdgeIndex,
            incidentEdgeIndex,
            incidentEdge.origin,
        };
        candidate[1] = (contactFeature);
    }

    auto validContacts = CheckContactValidity(referenceEdge, -referenceEdge.direction, candidate);

    return validContacts;
}


Physics::Physics(aunteater::EntityManager & aEntityManager)
{
    aEntityManager.getFamily<PhysicalBody>().registerObserver(this);

    //Initiliazing the query functions
    queryFunctions[ShapeType_Hull][ShapeType_Hull] = QueryFacePenetration;
}

void Physics::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    //Updating all the positions of the proxy bodies
    for (auto & body : constructedBodies)
    {
        body.synchronize();
        body.debugRender();
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

            auto aabbA = bodyA.box.shape.getAABB();
            auto aabbB = bodyB.box.shape.getAABB();

            debugDrawer->drawOutline({
                    aabbA.mPosition,
                    aabbA.mDimension,
                    math::Matrix<3, 3>::Identity(),
                    {0,255,255}
            });

            debugDrawer->drawOutline({
                    aabbB.mPosition,
                    aabbB.mDimension,
                    math::Matrix<3, 3>::Identity(),
                    {0,255,255}
            });

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
        collisionPair.debugRender();
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

            Shape::Edge refEdge = bodyRef->box.shape.getEdge(manifold.referenceEdgeIndex);

            manifold.incidentEdgeIndex = findIncidentEdge(refEdge.direction, bodyInc->box.shape);

            manifold.contacts = getContactPoints(
                manifold.referenceEdgeIndex,
                manifold.incidentEdgeIndex,
                bodyRef->box.shape,
                bodyInc->box.shape
            );

            //Now we check for persisted contact equivalence
            for (auto oldContact : oldManifold.contacts)
            {
                for (auto newContact : manifold.contacts)
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
            for (auto & contact : manifold.contacts)
            {
                velocityConstraints.emplace_back(VelocityConstraint{
                        bodyRef->speed,
                        bodyRef->massCenter,
                        bodyRef->position,
                        bodyRef->theta,
                        bodyRef->w,
                        bodyRef->invMass,
                        bodyRef->invMoi,
                        0.,
                        bodyInc->speed,
                        bodyInc->massCenter,
                        bodyInc->position,
                        bodyInc->theta,
                        bodyInc->w,
                        bodyInc->invMass,
                        bodyInc->invMoi,
                        0.,

                        sqrt(bodyRef->friction * bodyInc->friction),
                        std::max(bodyRef->friction, bodyInc->friction),
                        manifold.normal,
                        
                        contact,
                        bodyRef,
                        bodyInc
                });
            }
        }
        else
        {
            //Destroy pair here
            //collidingBodies.erase(pairIt++);
        }
    }
    

    //Solve constraints
    for (VelocityConstraint & constraint : velocityConstraints)
    {
        constraint.debugRender();
        // Warm starting the constraint
        ContactFeature & cf = constraint.cf;
        Vec2 rA = cf.contactPoint.as<math::Vec>() - (constraint.cA.as<math::Vec>());
        Vec2 rB = cf.contactPoint.as<math::Vec>() - (constraint.cB.as<math::Vec>());

        double crossA = twoDVectorCross(rA, constraint.normal);
        double crossB = twoDVectorCross(rB, constraint.normal);

        Vec2 impulseVecA = cf.normalImpulse * constraint.normal * constraint.invMassA;
        double angularImpulseA = cf.normalImpulse * crossA * constraint.invMoiA;


        Vec2 impulseVecB = -cf.normalImpulse * constraint.normal * constraint.invMassB;
        double angularImpulseB = -cf.normalImpulse * crossB * constraint.invMoiB;

        applyImpulse(
                impulseVecA,
                angularImpulseA,
                impulseVecB,
                angularImpulseB,
                constraint,
                aTimer.delta()
                );
    }

    for (int i = 0; i < maxNormalConstraintIteration; i++)
    {
        for (VelocityConstraint & constraint : velocityConstraints)
        {

            ContactFeature & cf = constraint.cf;
            Vec2 rA = cf.contactPoint.as<math::Vec>() - (constraint.cA.as<math::Vec>());
            Vec2 rB = cf.contactPoint.as<math::Vec>() - (constraint.cB.as<math::Vec>());
            Vec2 AngVecA = {-rA.y(), rA.x()};
            Vec2 AngVecB = {-rB.y(), rB.x()};

            Vec2 speed = constraint.vA + (AngVecA * constraint.wA) - constraint.vB - (AngVecB * constraint.wB);

            double totalMass = constraint.invMassA + constraint.invMassB;

            double normalSpeed = speed.dot(constraint.normal);

            double crossA = twoDVectorCross(rA, constraint.normal);
            double crossB = twoDVectorCross(rB, constraint.normal);
            double crossASquared = crossA * crossA;
            double crossBSquared = crossB * crossB;
            double totalAngularMass = constraint.invMoiA * crossASquared + constraint.invMoiB * crossBSquared;

            double lambda = -(1 / (totalMass + totalAngularMass)) * (normalSpeed);
            double newImpulse = std::max(cf.normalImpulse + lambda, 0.);
            lambda = newImpulse - cf.normalImpulse;
            cf.normalImpulse = newImpulse;

            Vec2 impulseVecA = cf.normalImpulse * constraint.normal * constraint.invMassA;
            double angularImpulseA = cf.normalImpulse * crossA * constraint.invMoiA;

            Vec2 impulseVecB = -cf.normalImpulse * constraint.normal * constraint.invMassB;
            double angularImpulseB = cf.normalImpulse * crossB * constraint.invMoiB;

            applyImpulse(
                    impulseVecA,
                    angularImpulseA,
                    impulseVecB,
                    angularImpulseB,
                    constraint,
                    aTimer.delta()
                    );

            if (constraint.friction > 0.)
            {
                Vec2 tangent = {constraint.normal.y(), -constraint.normal.x()};

                double tangentSpeed = speed.dot(tangent);

                double crossATangent = twoDVectorCross(rA, tangent);
                double crossBTangent = twoDVectorCross(rB, tangent);
                double crossATangentSquared = crossATangent * crossATangent;
                double crossBTangentSquared = crossBTangent * crossBTangent;

                double totalAngularTangentMass = constraint.invMoiA * crossATangentSquared + constraint.invMoiB * crossBTangentSquared;

                double lambda = -(1 / (totalMass + totalAngularTangentMass)) * (tangentSpeed);
                double maxFriction = constraint.friction * cf.normalImpulse;
                double newImpulse = std::max(std::min(cf.tangentImpulse + lambda, maxFriction), -maxFriction);
                lambda = newImpulse - cf.tangentImpulse;
                cf.tangentImpulse = newImpulse;


                Vec2 impVecA = lambda * tangent * constraint.invMassA;
                double angularImpulseA = lambda * crossATangent * constraint.invMoiA;


                Vec2 impVecB = -lambda * tangent * constraint.invMassB;
                double angularImpulseB = -lambda * crossBTangent * constraint.invMoiB;

                applyImpulse(
                        impulseVecA,
                        angularImpulseA,
                        impulseVecB,
                        angularImpulseB,
                        constraint,
                        aTimer.delta()
                        );
            }
        }
    }

    for (VelocityConstraint & constraint : velocityConstraints)
    {
        constraint.cbA->position = constraint.pA;
        constraint.cbA->theta = constraint.aA;
        constraint.cbA->speed = constraint.vA;
        constraint.cbA->w = constraint.wA;

        constraint.cbB->position = constraint.pB;
        constraint.cbB->theta = constraint.aB;
        constraint.cbB->speed = constraint.vB;
        constraint.cbB->w = constraint.wB;
    }

    //Update entities body
    for (auto & body : constructedBodies)
    {
        body.updateEntity();
    }

    //Cleaning up
    velocityConstraints.clear();
}

} // namespace grapito
} // namespace ad
