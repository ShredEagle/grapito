#pragma once

#include "Input.h"

#include <Components/Weight.h>
#include <Components/ForceAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Weight, ForceAndSpeed> Weightable;

class Gravity : public aunteater::System
{

public:
    Gravity(aunteater::Engine & aEngine);

    void update(const aunteater::Timer aTimer) override;

    void loadInputState(const gameInputState & aInputState);

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<Weightable> mWeightables;

};

} // namespace ad
