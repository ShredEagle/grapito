#include "ImpulseSolver.h"
#include "Utils/DrawDebugStuff.h"
#include "engine/commons.h"
#include "math/Vector.h"
#include <algorithm>
#include <iostream>

namespace ad {

ImpulseSolver::ImpulseSolver(aunteater::EntityManager & aEntityManager) :
    mImpulsables(aEntityManager)
{}

void applyImpulse(Position & pos, AccelAndSpeed & aas, math::Vec<2, double> impVec, double delta)
{
    pos.position += impVec * delta;
    aas.oldSpeed = aas.speed;
    aas.speed += impVec;
}

void ImpulseSolver::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for (auto impulsable : mImpulsables)
    {
        AccelAndSpeed & aas = impulsable->get<AccelAndSpeed>();
        Position & pos = impulsable->get<Position>();
        Body & ecb = impulsable->get<Body>();

        for (auto query : ecb.collidingWith)
        {
            for (auto contact : query.contacts)
            {
                applyImpulse(pos, aas, contact.impulse * query.normal * ecb.invMass, aTimer.delta());
            }
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        for (auto impulsable : mImpulsables)
        {
            AccelAndSpeed & aasA = impulsable->get<AccelAndSpeed>();
            Position & posA = impulsable->get<Position>();
            Body & ecbA = impulsable->get<Body>();

            for (auto query : ecbA.collidingWith)
            {
                AccelAndSpeed & aasB = query.entity->get<AccelAndSpeed>();
                Position & posB = query.entity->get<Position>();
                Body & ecbB = query.entity->get<Body>();

                for (auto contact : query.contacts)
                {
                    double normalSpeed = (aasA.speed - aasB.speed).dot(query.normal);
                    double totalMass = ecbA.invMass + ecbB.invMass;

                    double lambda = -(1 / totalMass) * (normalSpeed);
                    double newImpulse = std::max(contact.impulse + lambda, (double)0.f);
                    lambda = newImpulse - contact.impulse;
                    contact.impulse = newImpulse;


                    math::Vec<2, double> impVec = lambda * query.normal * ecbA.invMass;

                    debugDrawer->drawLine({
                            contact.point,
                            contact.point + impVec,
                            2.f,
                            Color{0,0,255}
                            });

                    applyImpulse(posA, aasA, impVec, aTimer.delta());

                    impVec = -lambda * query.normal * ecbB.invMass;
                    applyImpulse(posB, aasB, impVec, aTimer.delta());
                }
            }
        }

    }
}

} // namespace ad
