#include "EnvironmentCollision.h"

#include "Utils/CollisionBox.h"
#include <cmath>
#include <iostream>
#include <limits>

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
    //math::Vec<2, double> orthVector = point - projectPointOnLine(point, origin, end);
    //return orthVector.dot(normal);
    return (((end.x() - origin.x()) * (origin.y() - point.y())) - ((origin.x() - point.x()) * (end.y() - origin.y()))) / (origin - end).getNorm();
}

ContactPointQuery QueryFacePenetration(const CollisionBox & collisionBoxA, const CollisionBox & collisionBoxB)
{
    double bestDistance = -std::numeric_limits<double>::max();
    int bestIndex;
    math::Position<2, double> bestContact = math::Position<2, double>::Zero();
    math::Vec<2, double> bestNormal = math::Vec<2, double>::Zero();
    math::Position<2, double> bestOrigin = math::Position<2, double>::Zero();
    math::Position<2, double> bestEnd = math::Position<2, double>::Zero();
    math::Position<2, double> bestPoint = math::Position<2, double>::Zero();

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
            bestContact = support;
            bestNormal = edgeA.normal;
            bestOrigin = edgeA.origin;
            bestEnd = edgeA.end;
            bestPoint = pointOnLine;
        }
    }

    return {bestDistance, bestContact, bestNormal, bestOrigin, bestEnd, bestPoint};
}

EnvironmentCollision::EnvironmentCollision(aunteater::EntityManager & aEntityManager) :
    mColliders(aEntityManager)
{
    queryFunctions[CollisionBoxType::AABB][CollisionBoxType::AABB] = QueryFacePenetration;
}

void EnvironmentCollision::getBestQuery(
    ContactPointQuery & bestQuery,
    CollisionBoxType ecbAType,
    CollisionBoxType ecbBType,
    CollisionBox ecbA,
    CollisionBox ecbB
)
{
    ContactPointQuery queryA = queryFunctions[ecbBType][ecbAType](ecbB, ecbA);

    if (queryA.distance > 0.f)
    {
        return;
    }

    bestQuery = queryA;

    ContactPointQuery queryB = queryFunctions[ecbAType][ecbBType](ecbA, ecbB);

    if (queryB.distance > 0.f)
    {
        return;
    }
    
    if (queryB.distance < queryA.distance)
    {
        bestQuery = queryB;
    }
}


void EnvironmentCollision::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for(auto colliderA : mColliders)
    {
        EnvironmentCollisionBox & ecbA = colliderA->get<EnvironmentCollisionBox>();
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
                EnvironmentCollisionBox & ecbB = colliderB->get<EnvironmentCollisionBox>();
                Position & posB = colliderB->get<Position>();

                CollisionBox ecbTranslatedB{
                    math::Rectangle<double>{
                        colliderB->get<Position>().position + colliderB->get<EnvironmentCollisionBox>().box.mBox.mPosition.as<math::Vec>(),
                        colliderB->get<EnvironmentCollisionBox>().box.mBox.mDimension
                    }
                };

                ContactPointQuery bestQuery;

                getBestQuery(bestQuery, ecbA.ecbType, ecbB.ecbType, ecbTranslatedA, ecbTranslatedB);

                if (!std::isnan(bestQuery.distance))
                {
                    colliderA->get<EnvironmentCollisionBox>().collidingWith.push_back(ContactWith{bestQuery, colliderB});
                }
            }
        }
    }
}
} // namespace ad

