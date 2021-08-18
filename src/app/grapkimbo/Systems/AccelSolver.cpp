#include "AccelSolver.h"

namespace ad {

AccelSolver::AccelSolver(aunteater::EntityManager & aEntityManager) :
    mAccellerated(aEntityManager)
{}

void AccelSolver::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    // For the accel solver we will use a derive methode
    // from velocity verlet. It assumes a constant acceleration
    // for total precision but error with changing acceleration
    // are really small.
    // see: https://www.youtube.com/watch?v=hG9SzQxaCm8
    for(auto accelerated : mAccellerated)
    {
        AccelAndSpeed & aas = accelerated->get<AccelAndSpeed>();
        Position & pos = accelerated->get<Position>();

        for (auto accel : aas.accelerations)
        {
            pos.oldPosition = pos.position;
            pos.position += aas.speed * aTimer.delta() + .5f * accel * aTimer.delta() * aTimer.delta();
            aas.oldSpeed = aas.speed;
            aas.speed += accel * aTimer.delta();
        }

        aas.accelerations.clear();
    }
}

} // namespace ad
