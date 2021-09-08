#include "ImpulseSolver.h"
#include "Utils/DrawDebugStuff.h"
#include "commons.h"
#include "engine/commons.h"
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
    for (auto & [aas, pos, ecb] : mImpulsables)
    {
        for (auto query : ecb.collidingWith)
        {
            for (auto contact : query.contacts)
            {
                //TODO finish angular impulse persistence
                applyImpulse(pos, ecb, aas, contact.impulse * query.normal * ecb.invMass, 0., aTimer.delta());
            }
        }
    }

    for (int i = 0; i < 10; ++i)
    {
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
                    Vec2 AngVecA = {-rA.y(), rA.x()};
                    Vec2 AngVecB = {-rB.y(), rB.x()};

                    double normalSpeed = (aasA.speed + (AngVecA * aasA.w) - aasB.speed - (AngVecB * aasB.w)).dot(query.normal);
                    double totalMass = ecbA.invMass + ecbB.invMass;
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

                    std::cout << "lambda " <<  lambda << "\n";
                    std::cout << "totalMass " <<  totalMass << "\n";
                    std::cout << "totalAngularMass " <<  totalAngularMass << "\n";
                    std::cout << "normalSpeed " <<  normalSpeed << "\n";
                    std::cout << "aasSpeed " <<  aasA.speed << "\n";
                    std::cout << "massA " <<  ecbA.invMass << "\n";
                    std::cout << "contact impulse " <<  contact.impulse << "\n";
                    debugDrawer->drawLine({
                            contact.point, 
                            static_cast<Position2>(contact.point.as<math::Vec>() + impVec),
                            1.,
                            {255, 0, 0}
                            });

                    applyImpulse(posA, ecbA, aasA, impVec, angularImpulse, aTimer.delta());

                    impVec = -lambda * query.normal * ecbB.invMass;
                    angularImpulse = -lambda * crossB * ecbB.invMoi;
                    applyImpulse(posB, ecbB, aasB, impVec, angularImpulse, aTimer.delta());

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
