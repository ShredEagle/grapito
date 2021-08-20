#pragma once

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include "Input.h"
#include <Components/Position.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<AccelAndSpeed, Position, Body> Impulsable;

class ImpulseSolver : public aunteater::System<GameInputState>
{

public:
    ImpulseSolver(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<Impulsable> mImpulsables;

};

} // namespace ad
