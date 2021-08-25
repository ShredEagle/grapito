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

class SpeedResolution : public aunteater::System<GameInputState>
{

public:
    SpeedResolution(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Movable> mMovables;

};

} // namespace ad
