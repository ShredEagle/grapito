#include "Gravity.h"

#include <math/Vector.h>

namespace ad {

const math::Vec<2, double> gravityVector = {0., - Gravity::gAcceleration};

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mMassives(aEntityManager)
{}

void Gravity::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for(auto massive : mMassives)
    {
        massive->get<AccelAndSpeed>().accelerations.push_back(gravityVector);
    }
}
} // namespace ad
