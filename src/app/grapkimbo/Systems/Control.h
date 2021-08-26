#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/ForceAndSpeed.h>
#include <Components/Pendular.h>
#include <Components/Position.h>
#include <Components/Weight.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/EntityManager.h>
#include <aunteater/System.h>

#include <utility>

namespace ad {

typedef aunteater::Archetype<Controllable, Position, ForceAndSpeed, Weight> CartesianControlled;
typedef aunteater::Archetype<Controllable, Position, Pendular, Weight> PolarControlled;
// Currently using EnvironmentCollisionBox as a tag, because it is not correctly positioned...
typedef aunteater::Archetype<Position, EnvironmentCollisionBox> AnchorElement;

class Control : public aunteater::System<GameInputState>
{

public:
    Control(aunteater::EntityManager & mEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

    static constexpr double gPendularControlAccelerationFactor = 1./6.;
    static constexpr double gAirControlAcceleration = 12.; // m/s

private:
    std::pair<math::Position<2, double>, double> anchor(const math::Position<2, double> aPosition);

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<PolarControlled> mPolarControllables;
    const aunteater::FamilyHelp<AnchorElement> mAnchorables;

};

} // namespace ad
