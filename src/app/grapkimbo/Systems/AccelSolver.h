#pragma once

#include "Input.h"
#include "aunteater/EntityManager.h"
#include <Components/Position.h>
#include <Components/AccelAndSpeed.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<AccelAndSpeed, Position> Accelerated;

class AccelSolver : public aunteater::System<GameInputState>
{

public:
    AccelSolver(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<Accelerated> mAccellerated;

};

} // namespace grapito
} // namespace ad
