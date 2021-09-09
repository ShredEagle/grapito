#include "ImpulseSolver.h"
#include "Utils/DrawDebugStuff.h"
#include "commons.h"
#include "engine/commons.h"
#include "math/Interpolation.h"
#include "math/Vector.h"
#include <algorithm>
#include <iostream>

namespace ad {
namespace grapito
{

ImpulseSolver::ImpulseSolver(aunteater::EntityManager & aEntityManager) :
    mImpulsables(aEntityManager)
{}

void applyImpulse(Position & pos, Body & ecb, AccelAndSpeed & aas, Vec2 impVec, double angularImpulse, double delta)
{
    pos.position += impVec * delta;
    ecb.theta += math::Radian<double>{angularImpulse * delta};
    aas.oldSpeed = aas.speed;
    aas.speed += impVec;

    aas.w += angularImpulse;
}

void ImpulseSolver::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    /*
     * This is useless until we have contact point persistence
    for (auto & [aasA, posA, ecbA] : mImpulsables)
    {
        for (auto query : ecbA.collidingWith)
        {
            AccelAndSpeed & aasB = query.entity->get<AccelAndSpeed>();
            Position & posB = query.entity->get<Position>();
            Body & ecbB = query.entity->get<Body>();

            for (auto contact : query.contacts)
            {
                Vec2 rA = contact.point.as<math::Vec>() - (posA.position.as<math::Vec>() + ecbA.massCenter.as<math::Vec>());
                Vec2 rB = contact.point.as<math::Vec>() - (posB.position.as<math::Vec>() + ecbB.massCenter.as<math::Vec>());

                double crossA = twoDVectorCross(rA, query.normal);
                double crossB = twoDVectorCross(rB, query.normal);

                //Apply all impulse to ref body
                Vec2 impulse = contact.impulse * ecbA.invMass * query.normal;
                double angularImpulse = contact.impulse * crossA * ecbA.invMoi;

                applyImpulse(posA, ecbA, aasA, impulse, angularImpulse, aTimer.delta());

                Vec2 tangentImpulse = contact.tangentImpulse * ecbA.invMass * query.normal;
                double tangentAngularImpulse = contact.tangentImpulse * crossA * ecbA.invMoi;

                applyImpulse(posA, ecbA, aasA, tangentImpulse, tangentAngularImpulse, aTimer.delta());

                //Apply all impulse to incident body
                impulse = contact.impulse * ecbB.invMass * query.normal;
                angularImpulse = contact.impulse * crossB * ecbB.invMoi;

                applyImpulse(posB, ecbB, aasB, impulse, angularImpulse, aTimer.delta());

                tangentImpulse = contact.tangentImpulse * ecbB.invMass * query.normal;
                tangentAngularImpulse = contact.tangentImpulse * crossB * ecbB.invMoi;

                applyImpulse(posB, ecbB, aasB, tangentImpulse, tangentAngularImpulse, aTimer.delta());
            }
        }
    }
    */

    std::vector<std::tuple<AccelAndSpeed &, Position &, Body &>> impulsableVector;
    for (auto & [aasA, posA, ecbA] : mImpulsables)
    {
        impulsableVector.emplace_back(std::tie(aasA, posA, ecbA));
    }

    for (int i = 0; i < 50; ++i)
    {
        for (auto & [aasA, posA, ecbA] : impulsableVector)
        {
            for (auto query : ecbA.collidingWith)
            {
                AccelAndSpeed & aasB = query.entity->get<AccelAndSpeed>();
                Position & posB = query.entity->get<Position>();
                Body & ecbB = query.entity->get<Body>();

                for (auto contact : query.contacts)
                {
                    Vec2 rA = contact.point.as<math::Vec>() - (posA.position.as<math::Vec>() + ecbA.massCenter.as<math::Vec>());
                    Vec2 rB = contact.point.as<math::Vec>() - (posB.position.as<math::Vec>() + ecbB.massCenter.as<math::Vec>());
                    Vec2 AngVecA = {-rA.y(), rA.x()};
                    Vec2 AngVecB = {-rB.y(), rB.x()};

                    Vec2 speed = aasA.speed + (AngVecA * aasA.w) - aasB.speed - (AngVecB * aasB.w);

                    double totalMass = ecbA.invMass + ecbB.invMass;


                    double normalSpeed = speed.dot(query.normal);

                    double crossA = twoDVectorCross(rA, query.normal);
                    double crossB = twoDVectorCross(rB, query.normal);
                    double crossASquared = crossA * crossA;
                    double crossBSquared = crossB * crossB;
                    double totalAngularMass = ecbA.invMoi * crossASquared + ecbB.invMoi * crossBSquared;

                    double lambda = -(1 / (totalMass + totalAngularMass)) * (normalSpeed);
                    double newImpulse = std::max(contact.impulse + lambda, 0.);
                    lambda = newImpulse - contact.impulse;
                    contact.impulse = newImpulse;


                    Vec2 impVec = lambda * query.normal * ecbA.invMass;
                    double angularImpulse = lambda * crossA * ecbA.invMoi;

                    applyImpulse(posA, ecbA, aasA, impVec, angularImpulse, aTimer.delta());

                    impVec = -lambda * query.normal * ecbB.invMass;
                    angularImpulse = -lambda * crossB * ecbB.invMoi;
                    applyImpulse(posB, ecbB, aasB, impVec, angularImpulse, aTimer.delta());

                    if (ecbB.friction > 0. && ecbA.friction > 0.)
                    {
                        double friction = sqrt(ecbA.friction * ecbB.friction);
                        Vec2 tangent = {query.normal.y(), -query.normal.x()};

                        double tangentSpeed = speed.dot(tangent);

                        double crossATangent = twoDVectorCross(rA, tangent);
                        double crossBTangent = twoDVectorCross(rB, tangent);
                        double crossATangentSquared = crossATangent * crossATangent;
                        double crossBTangentSquared = crossBTangent * crossBTangent;

                        double totalAngularTangentMass = ecbA.invMoi * crossATangentSquared + ecbB.invMoi * crossBTangentSquared;

                        double lambda = -(1 / (totalMass + totalAngularTangentMass)) * (tangentSpeed);
                        double maxFriction = friction * contact.impulse;
                        double newImpulse = std::max(std::min(contact.tangentImpulse + lambda, maxFriction), -maxFriction);
                        lambda = newImpulse - contact.tangentImpulse;
                        contact.tangentImpulse = newImpulse;


                        Vec2 impVec = lambda * tangent * ecbA.invMass;
                        double angularImpulse = lambda * crossATangent * ecbA.invMoi;

                        applyImpulse(posA, ecbA, aasA, impVec, angularImpulse, aTimer.delta());

                        impVec = -lambda * tangent * ecbB.invMass;
                        angularImpulse = -lambda * crossBTangent * ecbB.invMoi;

                        applyImpulse(posB, ecbB, aasB, impVec, angularImpulse, aTimer.delta());
                    }

                    /*
                    debugDrawer->drawLine({
                            contact.point, 
                            static_cast<Position2>(contact.point.as<math::Vec>() + AngVecA * aasA.w),
                            1.,
                            {255, 0, 0}
                            });
                    debugDrawer->drawLine({
                            contact.point, 
                            static_cast<Position2>(contact.point.as<math::Vec>() - rA),
                            1.,
                            {255, 0, 255}
                            });
                    debugDrawer->drawLine({
                            contact.point, 
                            static_cast<Position2>(contact.point.as<math::Vec>() + normalSpeed * query.normal),
                            1.,
                            {0, 255, 0}
                            });
                            */
                }
            }
        }

    }
}

} // namespace grapito
} // namespace ad
