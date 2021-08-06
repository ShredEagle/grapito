#include "SpeedResolution.h"

#include <math/Vector.h>

namespace ad {

SpeedResolution::SpeedResolution(aunteater::Engine & aEngine) :
    mEngine(aEngine),
    mMovables(mEngine)
{}

void SpeedResolution::update(const aunteater::Timer aTimer)
{
    for(auto movable : mMovables)
    {
        math::Vec<2, float> totalSpeed = math::Vec<2, float>::Zero();
        ForceAndSpeed fas = movable->get<ForceAndSpeed>();

        for (auto speed : fas.speeds)
        {
            totalSpeed += speed;
        }

        for (auto force :fas.forces)
        {
            totalSpeed += force / movable->get<Weight>().weight;
        }

        fas.speeds.clear();
        fas.forces.clear();

        movable->get<Position>().position += static_cast<math::Vec<2,int>>(totalSpeed);

        fas.speeds.push_back(totalSpeed);
    }
}
} // namespace ad
