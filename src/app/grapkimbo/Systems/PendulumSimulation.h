#pragma once

#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <Components/Pendular.h>
#include <Components/Position.h>


namespace ad {


using Pendulum = aunteater::Archetype<Pendular, Position>;


class PendulumSimulation : public aunteater::System
{
public:
    PendulumSimulation(aunteater::Engine &aEngine);

    void update(const aunteater::Timer aTimer) override;

private:
    const aunteater::FamilyHelp<Pendulum> mPendulums;
};


} // namespace ad
