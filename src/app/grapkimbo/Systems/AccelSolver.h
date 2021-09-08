#pragma once

#include "Input.h"

#include <Components/AccelAndSpeed.h>
#include "Components/Body.h"
#include <Components/Position.h>
#include "Components/VisualRectangle.h"

#include "aunteater/EntityManager.h"
#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<AccelAndSpeed, Position> Accelerated;
typedef aunteater::Archetype<AccelAndSpeed, Body, VisualRectangle, Position> Rotationed;

class AccelSolver : public aunteater::System<GameInputState>
{

public:
    AccelSolver(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<Accelerated> mAccellerated;
    const aunteater::FamilyHelp<Rotationed> mRotationed;

};

} // namespace grapito
} // namespace ad
