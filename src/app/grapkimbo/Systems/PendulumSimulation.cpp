#include "PendulumSimulation.h"

#include "Systems/Gravity.h"
#include "Utils/DrawDebugStuff.h"
#include <iostream>


namespace ad {
namespace grapito
{

PendulumSimulation::PendulumSimulation(aunteater::EntityManager & aEntityManager) :
    mPendulums{aEntityManager}
{}


void PendulumSimulation::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for(auto & [pendular, geometry, aas] : mPendulums)
    {
        // Right now this is gonna be a normal constraint that's completely resolve
        // to a speed in this system.
        // However this should be not be done like that. This should setup a rope
        // constraint that needs to be resolved by the ImpulseSolver or the future
        // PositionSolver
        //
        // Some people will ask why, and they would be right. The reason is that
        // you can't mess with mee position like that you know. That prevents the
        // ImpulseSolver from doing its job that's all.
        Vec2 pendularVec = geometry.position - pendular.anchor;
        pendularVec.normalize();
        Vec2 pendularNormal = {-pendularVec.y(), pendularVec.x()};
        aas.speed = aas.speed.dot(pendularNormal) * pendularNormal;
        aas.accel = aas.accel.dot(pendularNormal) * pendularNormal;

        // TODO There is a slight drift of rope length this will be solved by a Length constraint

        /* Old code is here

        math::Radian<double> angularAcceleration{ 
            - (Gravity::gAcceleration / pendular.length) * sin(pendular.angle)
        };

        pendular.angularSpeed += 
            (pendular.angularAccelerationControl + angularAcceleration) * aTimer.delta();
        pendular.angle += pendular.angularSpeed * aTimer.delta();


        geometry.position = 
            pendular.anchor 
            + Vec2{sin(pendular.angle), -cos(pendular.angle)} * pendular.length
            - (geometry.dimension / 2.).as<math::Vec>()
            ;
        */
    }
}


} // namespace grapito
} // namespace ad
