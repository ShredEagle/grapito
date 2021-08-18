#pragma once

#include "Components/EnvironmentCollisionBox.h"
#include "Components/Mass.h"
#include "Input.h"
#include "aunteater/EntityManager.h"

#include <Components/Position.h>
#include <Components/AccelAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<AccelAndSpeed, Position, EnvironmentCollisionBox, Mass> Impulsable;

class ImpulseSolver : public aunteater::System<GameInputState>
{

public:
    ImpulseSolver(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<Impulsable> mImpulsables;

};

} // namespace ad
