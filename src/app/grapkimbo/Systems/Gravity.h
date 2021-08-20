#pragma once

#include "Input.h"

#include "Components/Body.h"
#include <Components/AccelAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<AccelAndSpeed, Body> Massive;

class Gravity : public aunteater::System<GameInputState>
{

public:
    Gravity(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

    void loadInputState(const GameInputState & aInputState);

    static constexpr double gAcceleration = 100.f;

private:
    const aunteater::FamilyHelp<Massive> mMassives;

};

} // namespace ad
