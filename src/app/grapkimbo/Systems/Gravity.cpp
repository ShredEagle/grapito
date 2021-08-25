#include "Gravity.h"

#include <math/Vector.h>

namespace ad {

const math::Vec<2, double> gravityVector = {0., - Gravity::gAcceleration};

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mWeightables{mEntityManager}
{}

void Gravity::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for(auto weightable : mWeightables)
    {
        weightable->get<ForceAndSpeed>().forces.push_back(gravityVector * weightable->get<Weight>().mass);
    }
}
} // namespace ad
