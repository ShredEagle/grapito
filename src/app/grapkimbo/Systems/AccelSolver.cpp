#include "AccelSolver.h"
#include "commons.h"

#include "Utils/HomogeneousTransformation.h"
#include "math/Matrix.h"
#include <iostream>

namespace ad {
namespace grapito
{

AccelSolver::AccelSolver(aunteater::EntityManager & aEntityManager) :
    mAccellerated(aEntityManager),
    mRotationed(aEntityManager)
{}

void AccelSolver::update(const GrapitoTimer aTimer, const GameInputState &)
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

        pos.oldPosition = pos.position;
        pos.position += aas.speed * aTimer.delta() + .5f * aas.accel * aTimer.delta() * aTimer.delta();
        aas.oldSpeed = aas.speed;
        aas.speed += aas.accel * aTimer.delta();

        aas.accel = Vec2::Zero();
    }

    for(auto & [aas, body] : mRotationed)
    {
        body.theta += math::Radian<float>{aas.w * aTimer.delta()};
    }
}

} // namespace grapito
} // namespace ad
