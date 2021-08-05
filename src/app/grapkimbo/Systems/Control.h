#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/Position.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Controllable, Position> Controlled;

class Control : public aunteater::System
{

public:
    Control(aunteater::Engine & aEngine);

    void update(const aunteater::Timer aTimer) override;

    void loadInputState(const gameInputState & aInputState);

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<Controlled> mControllables;
    gameInputState mInputState;

};

} // namespace ad
