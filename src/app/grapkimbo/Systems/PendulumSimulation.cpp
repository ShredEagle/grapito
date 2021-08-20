#include "PendulumSimulation.h"

#include "Systems/Gravity.h"


namespace ad {


PendulumSimulation::PendulumSimulation(aunteater::Engine &aEngine) :
    mPendulums{aEngine}
{}


void PendulumSimulation::update(const aunteater::Timer aTimer)
{
    for(auto & [pendular, geometry] : mPendulums)
    {
        math::Radian<double> angularAcceleration{ 
            - (Gravity::gAcceleration / pendular.length) * sin(pendular.angle)
        };

        pendular.angularSpeed += 
            (pendular.angularAccelerationControl + angularAcceleration) * aTimer.delta();

        pendular.angle += pendular.angularSpeed * aTimer.delta();

        geometry.position = 
            pendular.anchor 
            + math::Vec<2, double>{sin(pendular.angle), -cos(pendular.angle)} * pendular.length
            - (geometry.dimension / 2.).as<math::Vec>()
            ;
    }
}


} // namespace ad
