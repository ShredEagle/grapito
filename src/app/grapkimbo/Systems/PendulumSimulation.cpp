#include "PendulumSimulation.h"

#include "Systems/Gravity.h"


namespace ad {


PendulumSimulation::PendulumSimulation(aunteater::Engine &aEngine) :
    mPendulums{aEngine}
{}


void PendulumSimulation::update(const aunteater::Timer aTimer)
{
    for(auto & [pendular, position] : mPendulums)
    {
        math::Radian<double> angularAcceleration{ 
            - (Gravity::gAcceleration / pendular.length) * sin(pendular.angle)
        };

        //pendular.angularSpeed += angularAcceleration * aTimer.delta();
        pendular.angularSpeed += angularAcceleration;
        pendular.angle += pendular.angularSpeed * aTimer.delta();

        position.position = 
            pendular.anchor 
            + math::Vec<2, double>{sin(pendular.angle), -cos(pendular.angle)} * pendular.length;
    }
}


} // namespace ad
