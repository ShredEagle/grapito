#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/ForceAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Controllable, Position, ForceAndSpeed> CartesianControlled;
typedef aunteater::Archetype<Controllable, Position, Pendular> PolarControlled;

class Control : public aunteater::System
{

public:
    Control(aunteater::Engine & aEngine);

    void update(const aunteater::Timer aTimer) override;

    void loadInputState(const gameInputState & aInputState);

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<PolarControlled> mPolarControllables;
    gameInputState mInputState;

};

} // namespace ad
