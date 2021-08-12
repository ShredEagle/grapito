#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/ForceAndSpeed.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/Weight.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Controllable, Position, ForceAndSpeed, Weight> CartesianControlled;
typedef aunteater::Archetype<Controllable, Position, Pendular, Weight> PolarControlled;

class Control : public aunteater::System
{

public:
    Control(aunteater::Engine & aEngine);

    void update(const aunteater::Timer aTimer) override;

    void loadInputState(const gameInputState & aInputState);

    static constexpr double gPendularControlAccelerationFactor = 1./6.;
    static constexpr double gAirControlAcceleration = 12.; // m/s

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<PolarControlled> mPolarControllables;
    gameInputState mInputState;

};

} // namespace ad
