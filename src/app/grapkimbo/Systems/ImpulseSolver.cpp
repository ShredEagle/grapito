#include "ImpulseSolver.h"
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
        EnvironmentCollisionBox & ecb = impulsable->get<EnvironmentCollisionBox>();
        Mass & mass = impulsable->get<Mass>();

        for (auto query : ecb.collidingWith)
        {
            applyImpulse(pos, aas, query.contact.impulse * query.contact.normal / mass.mass, aTimer.delta());
        }
    }

    for (int i = 0; i < 20; ++i)
    {
        for (auto impulsable : mImpulsables)
        {
            AccelAndSpeed & aas = impulsable->get<AccelAndSpeed>();
            Position & pos = impulsable->get<Position>();
            EnvironmentCollisionBox & ecb = impulsable->get<EnvironmentCollisionBox>();
            Mass & mass = impulsable->get<Mass>();

            for (auto query : ecb.collidingWith)
            {
                math::Vec<2, double> effSpeed = aas.speed;
                double totalMass = 1 / mass.mass;

                if (query.entity->has<AccelAndSpeed>())
                {
                    effSpeed -= query.entity->get<AccelAndSpeed>().speed;
                }

                if (query.entity->has<Mass>())
                {
                    totalMass += 1 / query.entity->get<Mass>().mass;
                }

                double normalSpeed = effSpeed.dot(query.contact.normal);
                double lambda = -(1 / totalMass) * (normalSpeed + 0.01 * query.contact.distance);
                double newImpulse = std::max(query.contact.impulse + lambda, (double)0.f);
                lambda = newImpulse - query.contact.impulse;
                query.contact.impulse = newImpulse;

                math::Vec<2, double> impVec = lambda * query.contact.normal / mass.mass;

                applyImpulse(pos, aas, impVec, aTimer.delta());

                if (query.entity->has<Mass>() && query.entity->has<AccelAndSpeed>() && query.entity->has<Position>())
                {
                    impVec = -lambda * query.contact.normal / query.entity->get<Mass>().mass;
                    applyImpulse(query.entity->get<Position>(), query.entity->get<AccelAndSpeed>(), impVec, aTimer.delta());
                }
            }
        }

    }
}

} // namespace ad
