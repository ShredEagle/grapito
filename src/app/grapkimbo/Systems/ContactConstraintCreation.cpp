#include "ContactConstraintCreation.h"

#include "Utils/DrawDebugStuff.h"
#include "commons.h"
#include "../Utilities.h"

#include "Utils/CollisionBox.h"

#include "aunteater/globals.h"
#include "engine/commons.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

namespace ad {
namespace grapito
{

const Position2 projectPointOnLine(Position2 point, Position2 origin, Position2 end)
{
    Vec2 e1 = end - origin;
    Vec2 e2 = point - origin;

    Vec2 normalizedE1 = e1 / e1.getNorm();
    Vec2 normalizedE2 = e2 / e2.getNorm();

    double cos = normalizedE2.dot(normalizedE1);

    double projOnLine = cos * e2.getNorm();

    return origin + normalizedE1 * projOnLine;
}

const double distanceToLine(Position2 point, Position2 origin, Position2 end, Vec2 normal)
{
    return (((end.x() - origin.x()) * (origin.y() - point.y())) - ((origin.x() - point.x()) * (end.y() - origin.y()))) / (origin - end).getNorm();
}

// This create a basic query from a reference collisionBoxA
// and a possible incident collisionBoxB.
// it computes all the distance from the anti-normal of the egdes
// of collisionBoxA for all vertices of collisionBoxB
// it returns the edgeIndex of collisionBoxB and the maximum distance
// vertex of collisionBoxB
void QueryFacePenetration(
        ContactQuery & query,
        const CollisionBox & collisionBoxA,
        const CollisionBox & collisionBoxB,
        const Position2 massCenterA,
        const Position2 massCenterB,
        const math::Radian<double> thetaA,
        const math::Radian<double> thetaB
        )
{
    int bestIndex;
    double bestDistance = -std::numeric_limits<double>::max();
    Vec2 bestNormal = Vec2::Zero();
    Contact bestContact{{0.,0.}};

#ifdef KIMBO_DEBUG
    Position2 bestOrigin = Position2::Zero();
    Position2 bestEnd = Position2::Zero();
    Position2 bestPoint = Position2::Zero();
#endif

    for (int i = 0; i < collisionBoxA.mFaceCount; ++i)
    {
        const Edge edgeA = collisionBoxA.getEdge(i, thetaA, massCenterA);
        const Position2 support = collisionBoxB.getSupport(-edgeA.normal, thetaB, massCenterB);
        const double distance = distanceToLine(support, edgeA.origin, edgeA.end, edgeA.normal);

        Color color = distance > 0 ? Color{0, 200, 0} : Color{200, 0, 0};
#ifdef KIMBO_DEBUG
        const Position2 pointOnLine = projectPointOnLine(support, edgeA.origin, edgeA.end);
#endif

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
    std::cout << bestDistance << "\n";
#endif
    query.distance = bestDistance;
    query.normal = bestNormal;
    query.index = bestIndex;
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
    const CollisionBox & ecbA,
    const CollisionBox & ecbB,
    const Position2 massCenterA,
    const Position2 massCenterB,
    const math::Radian<double> thetaA,
    const math::Radian<double> thetaB
)
{
    ReferenceFace face = ReferenceFace::FACEA;
    ContactQuery queryA = bestQuery;
    ContactQuery queryB = bestQuery;

    queryFunctions[ecbBType][ecbAType](queryA, ecbA, ecbB, massCenterA, massCenterB, thetaA, thetaB);


    if (queryA.distance > 0.f)
    {
        return face;
    }

    queryFunctions[ecbAType][ecbBType](queryB, ecbB, ecbA, massCenterB, massCenterA, thetaB, thetaA);

    if (queryB.distance > 0.f)
    {
        return face;
    }
    
    // We want the highest distance to minimize displacement because those distance are
    // negative !
    if (queryB.distance > queryA.distance)
    {
        face = ReferenceFace::FACEB;
        bestQuery = queryB;
    }
    else 
    {
        face = ReferenceFace::FACEA;
        bestQuery = queryA;
    }

    return face;
}

struct Line
{
    Position2 origin;
    Vec2 direction;
};

// This can only be used if we know lineA and lineB
// intersect
// see https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// where lineA is P->P+R and lineB is Q->Q+S
bool findIntersectionPoint(Position2 & result, const Line lineA, const Line lineB)
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

void getContactPoints(
    ContactQuery & query,
    const CollisionBox & referenceBox,
    const CollisionBox & incidentBox,
    const Position2 massCenterRef,
    const Position2 massCenterInc,
    const math::Radian<double> thetaRef,
    const math::Radian<double> thetaInc
)
{
    std::vector<Contact> candidateContact;
    // TODO we should test and take into account the already found contactPoint query.contacts

    Edge referenceEdge = referenceBox.getEdge(query.index, thetaRef, massCenterRef);
    Vec2 edgeDirection = {referenceEdge.normal.y(), -referenceEdge.normal.x()}; 

    Line lineA{referenceEdge.origin, -referenceEdge.normal * 20.};
    Line lineB{referenceEdge.end, -referenceEdge.normal * 20.};

    Edge bestEdge{{0.,0.}, {0.,0.}, {0.,0.}};
    double bestDot = -std::numeric_limits<double>::max();

    // We want to find the most antiparallel edge to our referenceEdge
    // The edgeDirection direction is reversed according to the referenceEdge
    // (this is to get a correctly oriented reference frame)
    // so we compute the dot of all incident edges with the edgeDirection 
    // and we store the maximum
    for (int i = 0; i < incidentBox.mFaceCount; ++i)
    {
        Edge edge = incidentBox.getEdge(i, thetaInc, massCenterInc);
        
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
    Position2 intersectA{0., 0.};
    Position2 intersectB{0., 0.};
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
    Position2 projectedOrigin = {
        (referenceEdge.normal.y() * bestEdge.origin.x() - referenceEdge.normal.x() * bestEdge.origin.y()) /
            (referenceEdge.normal.y() * edgeDirection.x() - referenceEdge.normal.x() * edgeDirection.y()),
        (edgeDirection.y() * bestEdge.origin.x() - edgeDirection.x() * bestEdge.origin.y()) /
            (referenceEdge.normal.x() * edgeDirection.y() - referenceEdge.normal.y() * edgeDirection.x())
    };
    Position2 projectedEnd = {
        (referenceEdge.normal.y() * bestEdge.end.x() - referenceEdge.normal.x() * bestEdge.end.y()) /
            (referenceEdge.normal.y() * edgeDirection.x() - referenceEdge.normal.x() * edgeDirection.y()),
        (edgeDirection.y() * bestEdge.end.x() - edgeDirection.x() * bestEdge.end.y()) /
            (referenceEdge.normal.x() * edgeDirection.y() - referenceEdge.normal.y() * edgeDirection.x())
    };
    if (
            projectedOrigin.y() <= projectedReferenceOrigin.y() &&
            projectedOrigin.x() <= projectedReferenceOrigin.x() &&
            projectedOrigin.x() >= projectedReferenceEnd.x()
       )
    {
        candidateContact.push_back({bestEdge.origin});
    }

    if (
            projectedEnd.y() <= projectedReferenceOrigin.y() &&
            projectedEnd.x() <= projectedReferenceOrigin.x() &&
            projectedEnd.x() >= projectedReferenceEnd.x()
       )
    {
        candidateContact.push_back({bestEdge.end});
    }

    query.contacts = candidateContact;

    /*
    debugDrawer->drawLine(
            {
                bestEdge.origin,
                bestEdge.end,
                2.,
                Color{200, 100, 40}
            }
        );

    debugDrawer->drawLine(
            {
                lineB.origin,
                lineB.origin + lineA.direction * 5,
                2.,
                Color{40, 200, 200}
            }
        );
    debugDrawer->drawLine(
            {
                lineA.origin,
                lineA.origin + lineB.direction * 100,
                2.,
                Color{40, 200, 200}
            }
        );

    Vec2 baseOrigin = Vec2{10., 10.};
    debugDrawer->drawPoint({
            (Position2) baseOrigin + projectedOrigin.as<math::Vec>(),
            3.f,
            {200,20,200},
        });
    debugDrawer->drawPoint({
            (Position2) baseOrigin + projectedEnd.as<math::Vec>(),
            3.f,
            {200,20,200},
        });
    debugDrawer->drawPoint({
            (Position2)baseOrigin + projectedReferenceEnd.as<math::Vec>(),
            3.f,
            {200,100,20},
        });
    debugDrawer->drawPoint({
            (Position2)baseOrigin + projectedReferenceOrigin.as<math::Vec>(),
            3.f,
            {200,200,20},
        });

    debugDrawer->drawLine({
            (Position2)baseOrigin,
            (Position2)(edgeDirection * 100 + baseOrigin),
            2.f,
            Color{200, 255, 200},
            });

    debugDrawer->drawLine({
            (Position2)baseOrigin,
            (Position2)(referenceEdge.normal * 100 + baseOrigin),
            2.f,
            Color{255, 200, 200},
            });
        */
}


void ContactConstraintCreation::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{

    std::vector<std::tuple<CollisionBox, Position2, Position2, math::Radian<double>, Body, Body &, aunteater::weak_entity>> colliderVector;
    for(auto & colliderA : mColliders)
    {
        Body & ecbA = colliderA->get<Body>();
        Position & posA = colliderA->get<Position>();

        CollisionBox ecbTranslatedA{
            math::Rectangle<double>{
                posA.position + ecbA.box.mBox.mPosition.as<math::Vec>(),
                ecbA.box.mBox.mDimension
            }
        };

        aunteater::weak_entity entityA = colliderA;
        Position2 center = ecbTranslatedA.mBox.center();
        Position2 massCenter = static_cast<Position2>(posA.position.as<math::Vec>() + ecbA.massCenter.as<math::Vec>());

        colliderVector.emplace_back(std::tie(ecbTranslatedA, center, massCenter, ecbA.theta, ecbA, ecbA, entityA));
    }

    for (std::size_t i = 0; i != colliderVector.size() - 1; ++i)
    {
        auto & [ecbTranslatedA, centerA, massCenterA, thetaA, ecbA, ecbARef, colliderA] = colliderVector.at(i);
        ecbARef.collidingWith.clear();

        for(std::size_t j = i + 1; j != colliderVector.size(); ++j)
        {
            auto & [ecbTranslatedB, centerB, massCenterB, thetaB, ecbB, ecbBRef, colliderB] = colliderVector.at(j);

            if (ecbB.bodyType != BodyType::STATIC || ecbA.bodyType != BodyType::STATIC)
            {

                const double radiusSum = std::pow((ecbA.radius + ecbB.radius), 2);

                // TODO This take way more time than it should even in O3
                // There is definitely something fishy about it but I don't know what
                // as a baseline line on 455 * 455 iteration this took more than 10ms
                /*
                const double distance = (ecbTranslatedA.mBox.center().as<math::Vec>() - ecbTranslatedB.mBox.center().as<math::Vec>()).getNormSquared();
                
                As a benchmark the code below runs in 145xxms
                This is approximately 164 cycle per iteration on my (franz) computer

                the code above runs in 175xxms
                This is approximately 246 cycle per iteration on my (franz) computer

                This is for "basically" 2 subtraction 2 addition and 2 multiplication

                And without any of it it takes 23xxms

                const math::Position<2, double> centerA = ecbTranslatedA.mBox.center();
                const math::Position<2, double> centerB = ecbTranslatedB.mBox.center();
                const double x1 = centerA.x();
                const double y1 = centerA.y();
                const double x2 = centerB.x();
                const double y2 = centerB.y();

                const double distance = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
                */

                const double x1 = centerA.x();
                const double y1 = centerA.y();
                const double x2 = centerB.x();
                const double y2 = centerB.y();

                const double distance = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);

                if (radiusSum > distance)
                {
                    ContactQuery bestQuery{colliderB};

                    ReferenceFace face = getBestQuery(
                            bestQuery,
                            ecbA.shapeType,
                            ecbB.shapeType,
                            ecbTranslatedA,
                            ecbTranslatedB,
                            massCenterA,
                            massCenterB,
                            thetaA,
                            thetaB
                            );

                    if (bestQuery.index != -1)
                    {
                        if (face == ReferenceFace::FACEA)
                        {
                            getContactPoints(bestQuery, ecbTranslatedA, ecbTranslatedB, massCenterA, massCenterB, thetaA, thetaB);
                        }
                        else
                        {
                            getContactPoints(bestQuery, ecbTranslatedB, ecbTranslatedA, massCenterB, massCenterA, thetaB, thetaA);
                            bestQuery.normal = -bestQuery.normal;
                        }

                        ecbARef.collidingWith.push_back(bestQuery);
                    }
                }
            }
        }
    }
}
} // namespace grapito
} // namespace ad

