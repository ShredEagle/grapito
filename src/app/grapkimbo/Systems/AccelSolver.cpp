#include "AccelSolver.h"
#include "commons.h"

#include "Utils/HomogeneousTransformation.h"
#include "math/Matrix.h"

namespace ad {
namespace grapito
{

AccelSolver::AccelSolver(aunteater::EntityManager & aEntityManager) :
    mAccellerated(aEntityManager),
    mRotationed(aEntityManager)
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

        pos.oldPosition = pos.position;
        pos.position += aas.speed * aTimer.delta() + .5f * aas.accel * aTimer.delta() * aTimer.delta();
        aas.oldSpeed = aas.speed;
        aas.speed += aas.accel * aTimer.delta();

        aas.accel = Vec2::Zero();
    }

    for(auto & [aas, body, vis, pos] : mRotationed)
    {
        body.theta += math::Radian<double>{aas.w * aTimer.delta()};
        vis.transform = static_cast<math::Matrix<3, 3, float>>(
                createPrefixedTransform(
                    body.theta,
                    static_cast<Position2>(pos.position.as<math::Vec>() + body.massCenter.as<math::Vec>())
                    )
                );
    }
}

} // namespace grapito
} // namespace ad
