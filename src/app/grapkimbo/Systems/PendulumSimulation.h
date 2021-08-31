#pragma once

#include "../Input.h"

#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include "Components/AccelAndSpeed.h"
#include <Components/Pendular.h>
#include <Components/Position.h>


namespace ad {


using Pendulum = aunteater::Archetype<Pendular, Position, AccelAndSpeed>;


class PendulumSimulation : public aunteater::System<GameInputState>
{
public:
    PendulumSimulation(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<Pendulum> mPendulums;
};


} // namespace ad
