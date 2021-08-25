#pragma once

#include "Input.h"

#include <Components/Weight.h>
#include <Components/ForceAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Weight, ForceAndSpeed> Weightable;

class Gravity : public aunteater::System<GameInputState>
{

public:
    Gravity(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

    void loadInputState(const GameInputState & aInputState);

    static constexpr double gAcceleration = 9.8;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Weightable> mWeightables;

};

} // namespace ad
