#include "Gravity.h"

#include <math/Vector.h>

namespace ad {

const math::Vec<2, float> gravityVector = {0.f, -1.f};

Gravity::Gravity(aunteater::Engine & aEngine) :
    mEngine(aEngine),
    mWeightables(mEngine)
{}

void Gravity::update(const aunteater::Timer aTimer)
{
    for(auto weightable : mWeightables)
    {
        weightable->get<ForceAndSpeed>().forces.push_back(gravityVector * weightable->get<Weight>().weight);
    }
}
} // namespace ad
