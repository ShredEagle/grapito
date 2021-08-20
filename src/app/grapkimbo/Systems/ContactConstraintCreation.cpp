#include "ContactConstraintCreation.h"

#include "Utils/CollisionBox.h"
#include "engine/commons.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

namespace ad {

const math::Position<2, double> projectPointOnLine(math::Position<2, double> point, math::Position<2, double> origin, math::Position<2, double> end)
{
    math::Vec<2, double> e1 = end - origin;
    math::Vec<2, double> e2 = point - origin;

    math::Vec<2, double> normalizedE1 = e1 / e1.getNorm();
    math::Vec<2, double> normalizedE2 = e2 / e2.getNorm();

    double cos = normalizedE2.dot(normalizedE1);

    double projOnLine = cos * e2.getNorm();

    return origin + normalizedE1 * projOnLine;
}

const double distanceToLine(math::Position<2, double> point, math::Position<2, double> origin, math::Position<2, double> end, math::Vec<2, double> normal)
{
    math::Vec<2, double> orthVector = point - projectPointOnLine(point, origin, end);
    return orthVector.dot(normal);
    //return (((end.x() - origin.x()) * (origin.y() - point.y())) - ((origin.x() - point.x()) * (end.y() - origin.y()))) / (origin - end).getNorm();
}

// This create a basic query from a reference collisionBoxA
// and a possible incident collisionBoxB.
// it computes all the distance from the anti-normal of the egdes
// of collisionBoxA for all vertices of collisionBoxB
// it returns the edgeIndex of collisionBoxB and the maximum distance
// vertex of collisionBoxB
void QueryFacePenetration(ContactQuery & query, const CollisionBox & collisionBoxA, const CollisionBox & collisionBoxB)
{
    int bestIndex;
    double bestDistance = -std::numeric_limits<double>::max();
    math::Vec<2, double> bestNormal = math::Vec<2, double>::Zero();
    Contact bestContact{{0.,0.}};

#ifdef KIMBO_DEBUG
    math::Position<2, double> bestOrigin = math::Position<2, double>::Zero();
    math::Position<2, double> bestEnd = math::Position<2, double>::Zero();
    math::Position<2, double> bestPoint = math::Position<2, double>::Zero();
#endif

    for (int i = 0; i < collisionBoxA.mFaceCount; ++i)
    {
        const Edge edgeA = collisionBoxA.getEdge(i);
        const math::Position<2, double> support = collisionBoxB.getSupport(-edgeA.normal);
        const double distance = distanceToLine(support, edgeA.origin, edgeA.end, edgeA.normal);
        const math::Position<2, double> pointOnLine = projectPointOnLine(support, edgeA.origin, edgeA.end);

        if (distance > bestDistance)
        {
            bestDistance = distance;
            bestIndex = i;
            bestNormal = -edgeA.normal;
            bestContact = {support};
#ifdef KIMBO_DEBUG
            bestOrigin = edgeA.origin;
            bestEnd = edgeA.end;
            bestPoint = pointOnLine;
#endif
        }
    }

#ifdef KIMBO_DEBUG
    query.origin = bestOrigin;
    query.end = bestEnd;
    query.point = bestPoint;
#endif
    query.distance = bestDistance;
    query.normal = bestNormal;
    query.index = bestIndex;
    query.contacts = {bestContact};
};

ContactConstraintCreation::ContactConstraintCreation(aunteater::EntityManager & aEntityManager) :
    mColliders(aEntityManager)
{
    queryFunctions[ShapeType::HULL][ShapeType::HULL] = QueryFacePenetration;
}

ReferenceFace ContactConstraintCreation::getBestQuery(
    ContactQuery & bestQuery,
    ShapeType ecbAType,
    ShapeType ecbBType,
    CollisionBox ecbA,
    CollisionBox ecbB
)
{
    ReferenceFace face = ReferenceFace::FACEA;
    ContactQuery queryA = bestQuery;
    ContactQuery queryB = bestQuery;

    queryFunctions[ecbBType][ecbAType](queryA, ecbA, ecbB);

    if (queryA.distance > 0.f)
    {
        return face;
    }

    bestQuery = queryA;

    queryFunctions[ecbAType][ecbBType](queryB, ecbB, ecbA);

    if (queryB.distance > 0.f)
    {
        return face;
    }
    
    if (queryB.distance < queryA.distance)
    {
        face = ReferenceFace::FACEB;
        bestQuery = queryB;
    }

    return face;
}

struct Line
{
    math::Position<2, double> origin;
    math::Vec<2, double> direction;
};

double intersectCross(math::Vec<2, double> v, math::Vec<2, double> w)
{
    return v.x() * w.y() - v.y() * w.x();
}

// This can only be used if we know lineA and lineB
// intersect
// see https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// where lineA is P->P+R and lineB is Q->Q+S
bool findIntersectionPoint(math::Position<2, double> & result, const Line lineA, const Line lineB)
{
    double rCrossS = intersectCross(lineA.direction, lineB.direction);

    math::Vec<2, double> base = lineB.origin - lineA.origin;
    double t = intersectCross(base, lineB.direction) / rCrossS;
    double u = intersectCross(base, lineA.direction) / rCrossS;

    if (t < 0. || t > 1. || u < 0. || u > 1)
    {
        return false;
    }

    result = lineA.origin + t * lineA.direction;

    return true;
}

void getContactPoints(
    ContactQuery & query,
    const CollisionBox referenceBox,
    const CollisionBox incidentBox
)
{
    std::vector<Contact> candidateContact;
    // TODO we should the already found contactPoint query.contacts

    Edge referenceEdge = referenceBox.getEdge(query.index);
    math::Vec<2, double> edgeDirection = {referenceEdge.normal.y(), -referenceEdge.normal.x()}; 

    Line lineA{referenceEdge.origin - referenceEdge.normal * 10000., referenceEdge.normal * 20000.};
    Line lineB{referenceEdge.end - referenceEdge.normal * 10000., referenceEdge.normal * 20000};

    Edge bestEdge{{0.,0.}, {0.,0.}, {0.,0.}};
    double bestDot = -std::numeric_limits<double>::max();

    // We want to find the most antiparallel edge to our referenceEdge
    // The edgeDirection direction is reversed according to the referenceEdge
    // (this is to get a correctly oriented reference frame)
    // so we compute the dot of all incident edges with the edgeDirection 
    // and we store the maximum
    for (int i = 0; i < incidentBox.mFaceCount; ++i)
    {
        Edge edge = incidentBox.getEdge(i);
        
        double dot = edgeDirection.dot(edge.end - edge.origin);

        if (dot > bestDot)
        {
            bestDot = dot;
            bestEdge = edge;
        }
    }

    // If the dot is 0 or less this means the two faces are either
    // orthogonal or back facing
    if (bestDot <= 0.)
    {
        return;
    }

    // We then find all intersecting point between the clipping lines and the
    // best incident edge
    math::Position<2, double> intersectA{0., 0.};
    math::Position<2, double> intersectB{0., 0.};
    Line incidentLine{bestEdge.origin, bestEdge.end - bestEdge.origin};

    bool hasInterA = findIntersectionPoint(intersectA, lineA, incidentLine);
    bool hasInterB = findIntersectionPoint(intersectB, lineB, incidentLine);

    if (hasInterA)
    {
        candidateContact.push_back({intersectA});
    }

    if (hasInterB)
    {
        candidateContact.push_back({intersectB});
    }

    // We use this algorithm to find all vertices with bit value (1|0)000
    // https://www.csd.uwo.ca/~sbeauche/CS3388/CS3388-2D-Clipping.pdf
    // Our y values are reversed because we use bottom right origin
    // This tests gives us all point beyond our colliding axis
    math::Position<2, double> projectedReferenceOrigin = static_cast<math::Position<2, double>>(
        referenceEdge.origin.as<math::Vec>().dot(edgeDirection) * edgeDirection +
        referenceEdge.origin.as<math::Vec>().dot(referenceEdge.normal) * referenceEdge.normal
        );
    math::Position<2, double> projectedReferenceEnd = static_cast<math::Position<2, double>>(
        referenceEdge.end.as<math::Vec>().dot(edgeDirection) * edgeDirection +
        referenceEdge.end.as<math::Vec>().dot(referenceEdge.normal) * referenceEdge.normal
        );
    math::Position<2, double> projectedOrigin = static_cast<math::Position<2, double>>(
        bestEdge.origin.as<math::Vec>().dot(edgeDirection) * edgeDirection +
        bestEdge.origin.as<math::Vec>().dot(referenceEdge.normal) * referenceEdge.normal
        );

    if (
            projectedOrigin.y() <= projectedReferenceOrigin.y() &&
            projectedOrigin.x() <= projectedReferenceOrigin.x() &&
            projectedOrigin.x() >= projectedReferenceEnd.x()
       )
    {
        candidateContact.push_back({projectedOrigin});
    }

    math::Position<2, double> projectedEnd = static_cast<math::Position<2, double>>(
        bestEdge.end.as<math::Vec>().dot(edgeDirection) * edgeDirection +
        bestEdge.end.as<math::Vec>().dot(referenceEdge.normal) * referenceEdge.normal
        );

    if (
            projectedEnd.y() <= projectedReferenceOrigin.y() &&
            projectedEnd.x() <= projectedReferenceOrigin.x() &&
            projectedEnd.x() >= projectedReferenceEnd.x()
       )
    {
        candidateContact.push_back({projectedEnd});
    }

    query.contacts = candidateContact;
}


void ContactConstraintCreation::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for(auto colliderA : mColliders)
    {
        Body & ecbA = colliderA->get<Body>();
        Position & posA = colliderA->get<Position>();
        ecbA.collidingWith.clear();

        CollisionBox ecbTranslatedA{
            math::Rectangle<double>{
                posA.position + ecbA.box.mBox.mPosition.as<math::Vec>(),
                ecbA.box.mBox.mDimension
            }
        };

        for(auto colliderB : mColliders)
        {
            if (colliderB != colliderA)
            {
                Body & ecbB = colliderB->get<Body>();
                Position & posB = colliderB->get<Position>();

                CollisionBox ecbTranslatedB{
                    math::Rectangle<double>{
                        colliderB->get<Position>().position + colliderB->get<Body>().box.mBox.mPosition.as<math::Vec>(),
                        colliderB->get<Body>().box.mBox.mDimension
                    }
                };

                ContactQuery bestQuery{colliderB};

                ReferenceFace face = getBestQuery(bestQuery, ecbA.shapeType, ecbB.shapeType, ecbTranslatedA, ecbTranslatedB);

                if (bestQuery.index != -1)
                {
                    if (face == ReferenceFace::FACEA)
                    {
                        getContactPoints(bestQuery, ecbTranslatedA, ecbTranslatedB);
                    }
                    else
                    {
                        getContactPoints(bestQuery, ecbTranslatedB, ecbTranslatedA);
                    }

                    colliderA->get<Body>().collidingWith.push_back(bestQuery);
                }
            }
        }
    }
}
} // namespace ad

