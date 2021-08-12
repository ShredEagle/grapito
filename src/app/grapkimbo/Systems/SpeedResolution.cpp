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
        math::Vec<2, double> totalSpeed = math::Vec<2, double>::Zero();
        ForceAndSpeed fas = movable->get<ForceAndSpeed>();

        for (auto speed : fas.speeds)
        {
            totalSpeed += speed;
        }

        for (auto force :fas.forces)
        {
            totalSpeed += (force / movable->get<Weight>().mass) * aTimer.delta();
        }

        fas.speeds.clear();
        fas.forces.clear();

        movable->get<Position>().position += totalSpeed * aTimer.delta();

        fas.speeds.push_back(totalSpeed);
    }
}
} // namespace ad
