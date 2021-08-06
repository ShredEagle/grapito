#pragma once

#include "Input.h"

#include <Components/Position.h>
#include <Components/ForceAndSpeed.h>
#include <Components/Weight.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Position, ForceAndSpeed, Weight> Movable;

class SpeedResolution : public aunteater::System
{

public:
    SpeedResolution(aunteater::Engine & aEngine);

    void update(const aunteater::Timer aTimer) override;

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<Movable> mMovables;

};

} // namespace ad
